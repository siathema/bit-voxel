#include "voxel.hpp"
#include "assets.hpp"
#include "array.hpp"
#include "stdlib.h"
#include "stdio.h"

namespace SMOBA
{
    
    Voxel_Chunk* Voxel_Chunk_Read_Hash(i32 chunkX, i32 chunkY, HashNode** table)
    {
		u64 key = (u64)chunkX << 32;
		key |= (u32)chunkY;
        Voxel_Chunk* result = (Voxel_Chunk*)HashRead((u8*)(&key), sizeof(u64), table);
        return result;
    }

    void Voxel_Chunk_Write_Hash(i32 chunkX, i32 chunkY, Voxel_Chunk* chunk,  HashNode** table)
    {
        u64 key = (u64)chunkX << 32 | (u32)chunkY;
        HashStore((u8*)(&key), sizeof(u64), chunk, table);
    }

    void Voxel_Chunk_Delete_Hash(i32 chunkX, i32 chunkY, HashNode** table)
    {
        u64 key = (u64)chunkX << 32 | (u32)chunkY;
        HashDelete((u8*)(&key), sizeof(u64), table);
    }

	Voxel_Frozen_Chunk Voxel_Chunk_Freeze(Voxel_Chunk* chunk)
	{

		Voxel_Frozen_Chunk result = { sizeof(Voxel_Chunk), (u8*)chunk };
		Mesh* mesh = &ASSETS::Meshes[chunk->MeshID];
		//Destroy_Mesh(mesh);
		//TODO(matthias): Remember to reuse Mesh slot.
		return result;
	}

	void Voxel_Chunk_Write_To_Disk(i32 chunkX, i32 chunkY, Voxel_World* world)
	{
		Voxel_Chunk* chunk = Voxel_Chunk_Read_Hash(chunkX, chunkY, (HashNode**)world->ChunkHashMap);
		Voxel_Frozen_Chunk frozenChunk = Voxel_Chunk_Freeze(chunk);

		Chunk_File_Header header = {};
		Chunk_File_Index index;
		u64 chunkFileLoc = sizeof(Chunk_File_Header);

		FILE* file = fopen("test.dat", "r+b");
		if (file != 0)
		{
			fread(&header, sizeof(Chunk_File_Header), 1, file);
			s_assert(header.Magic == CHUNK_MAGIC, "Not a valid ChunkFile!");
			fseek(file, header.IndexOffset, SEEK_SET);
			fread(&index, sizeof(Chunk_File_Index), 1, file);
		}
		else
		{
			file = fopen("test.dat", "w+b");
			header.IndexOffset = sizeof(Chunk_File_Header);
			index.NumEntries = 0;
		}
		bool found = false;

		if (index.NumEntries + 1 < CHUNK_FILE_INDEX_COUNT )
		{
			if (index.NumEntries - 1 != 0)
			{
				for (i32 entry = 0; entry < index.NumEntries; entry++)
				{
					if (index.Entries[entry].X == chunkX && index.Entries[entry].Y == chunkY)
					{
						chunkFileLoc += index.Entries[entry].Offset;
						found = true;
						break;
					}
				}
			}
		}
		else
		{
			s_assert(false, "NOT IMPLEMENTED");
		}

		if (!found)
		{
			index.Entries[index.NumEntries].X = chunkX;
			index.Entries[index.NumEntries].Y = chunkY;
			chunkFileLoc = index.Entries[index.NumEntries].Offset = sizeof(Chunk_File_Header) + header.Size;
			header.Size += frozenChunk.Size;
			header.IndexOffset += frozenChunk.Size;
			index.NumEntries++;
		}
		if (index.NumEntries - 1 == 0)
		{
			fseek(file, 0, SEEK_SET);
			fwrite(&header, sizeof(Chunk_File_Header), 1, file);
		}
		fseek(file, chunkFileLoc, SEEK_SET);
		fwrite(frozenChunk.Data, 1, frozenChunk.Size, file);
		if (!found)
		{
			fseek(file, 0, SEEK_SET);
			fwrite(&header, sizeof(Chunk_File_Header), 1, file);
			fseek(file, header.IndexOffset, SEEK_SET);
			fwrite(&index, sizeof(Chunk_File_Index), 1, file);
		}
		fclose(file);

	}

	void Voxel_Chunk_Read_From_Disk(i32 chunkX, i32 chunkY, Voxel_World* world)
	{

	}

	void Remove_Block(Voxel_World* world, i32 chunkX, i32 chunkY, i32 blockX, i32 blockY, i32 blockZ)
	{
		Voxel_Chunk* chunk = Voxel_Chunk_Read_Hash(chunkX, chunkY, (HashNode**)&world->ChunkHashMap);

		i32 blockIndex = (blockZ * (CHUNK_WIDTH*CHUNK_WIDTH)) + (blockY * CHUNK_WIDTH) + blockX;
		chunk->Blocks[blockIndex].BlockType = Air;
	}

	void Dig_Caves(Voxel_World* world, i32 chunkX, i32 chunkY)
	{
		PerlinData p = Init_Perlin(94302831);
		r64 genStep = 0.09;

        Voxel_Chunk* currentChunk = Voxel_Chunk_Read_Hash(chunkX, chunkY, (HashNode**)&world->ChunkHashMap);

        for(i32 x=0; x<CHUNK_WIDTH; x++)
        {
            for(i32 y=0; y<CHUNK_WIDTH; y++)
            {
                for(i32 z=0; z<CHUNK_HEIGHT; z++)
                {
					r64 stepX = ((x + (chunkX * CHUNK_WIDTH)) * genStep);
					r64 stepY = ((y + (chunkY * CHUNK_WIDTH)) * genStep);
                    r64 heightDouble =
                        Octave_Noise_Zero_To_One(stepX,
                                                 stepY,
                                                 z*genStep,
                                                 3, &p);
                    if(heightDouble < 0.25)
                    {
                        Remove_Block(world, chunkX, chunkY, x, y, z);
                    }
                }
            }
        }
	}

    void Gen_Chunk(Voxel_World* world, i32 chunkX, i32 chunkY)
    {
        u32 chunk = world->ChunkSize;
        u8* heightMap = Generate_Chunk_HeightMap(chunkX, chunkY);
        Generate_HeightMap_Voxel_Chunk(heightMap, &(world->Chunks[chunk]));
        world->Chunks[chunk].MeshID = 0;
        world->Chunks[chunk].generate = true;
        world->Chunks[chunk].WorldPosX = chunkX;
        world->Chunks[chunk].WorldPosY = chunkY;
        world->ChunkSize++;
        Voxel_Chunk_Write_Hash(chunkX, chunkY, &world->Chunks[chunk], (HashNode**)&world->ChunkHashMap);
        Dig_Caves(world, chunkX, chunkY);
        free(heightMap);
    }

	Voxel_World* Generate_Voxel_World()
	{
        Voxel_World* result = (Voxel_World*)calloc(1, sizeof(Voxel_World));
		u32 numChunks = (CHUNK_GEN_DIAMETER) * (CHUNK_GEN_DIAMETER);

		i32 currentChunkX = -(CHUNK_GEN_RADIUS);
		i32 currentChunkY = -(CHUNK_GEN_RADIUS);
        r32 radiusSquared = CHUNK_GEN_RADIUS * CHUNK_GEN_RADIUS;
		for (i32 chunk = 0; chunk < numChunks; chunk++)
		{
            r32 distanceSquared = currentChunkX*currentChunkX + currentChunkY*currentChunkY;
            if(distanceSquared <= radiusSquared)
            {
                Gen_Chunk(result, currentChunkX, currentChunkY);
            }
			currentChunkX++;
			if (currentChunkX == (CHUNK_GEN_RADIUS))
			{
				currentChunkX = -(CHUNK_GEN_RADIUS);
				currentChunkY++;
			}
		}

        return result;
	}

	u8* Generate_Chunk_HeightMap(i32 ChunkPosX, i32 ChunkPosY)
	{
		i32 size = CHUNK_WIDTH;
		i32 bytes = size * size;
		u8* result = (u8*)malloc(sizeof(u8)*bytes);

		PerlinData p = Init_Perlin(32);
		r64 genStep = 0.022;

		for (i32 row = 0; row < size; row++) {
			for (i32 col = 0; col < size; col++) {
				u32 index = ((row*size) + col);
				r64 stepX = ((col + (ChunkPosX * CHUNK_WIDTH)) * genStep);
				r64 stepY = ((row + (ChunkPosY * CHUNK_WIDTH)) * genStep);
				r64 heightDouble =
                    Octave_Noise(stepX,
                                        stepY,
                                        3, &p);
				u8 heightByte = GROUND_MIN + ((GROUND_MAX - GROUND_MIN)*heightDouble);

				result[index] = heightByte;
			}
		}
		return result;
	}

    Voxel_Block* Genertate_Voxel_Block(Voxel_Block* block, BlockType type)
    {
        block->BlockType = type;

        return block;
    }

	Voxel_Chunk* Generate_HeightMap_Voxel_Chunk(u8* heightMap, Voxel_Chunk* chunk)
	{
		for (i32 row = 0; row < CHUNK_WIDTH; row++)
		{
			for (i32 col = 0; col < CHUNK_WIDTH; col++)
			{
				u32 index = (row*CHUNK_WIDTH) + col;
				u8 height = heightMap[index];
				for (i32 i = 0; i < height; i++)
				{
                    BlockType type = Air;

                    if(i == height - 1) {
                        Genertate_Voxel_Block(
                            &chunk->Blocks[index + ((CHUNK_WIDTH*CHUNK_WIDTH)*i)], Grass);
                    }
                    else if(i >= height - 8)
                    {
                        Genertate_Voxel_Block(
                            &chunk->Blocks[index + ((CHUNK_WIDTH*CHUNK_WIDTH)*i)], Dirt);
                    }
                    else
                    {
                        Genertate_Voxel_Block(
                            &chunk->Blocks[index + ((CHUNK_WIDTH*CHUNK_WIDTH)*i)], Stone);
                    }
				}

			}
			//printf("\n");
		}
		return chunk;
	}

//Front Right Top
	static Vertex cube[] = {
        // NOTE(matthias): Front 0
		{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
        { 0.0f,  BLOCK_STEP, 0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
        // NOTE(matthias): Back 6
        { 0.0f, 0.0f,  BLOCK_STEP, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP,  BLOCK_STEP, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f,  BLOCK_STEP, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP,  BLOCK_STEP, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,  BLOCK_STEP, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f,  BLOCK_STEP,  BLOCK_STEP, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f },

        // NOTE(matthias): Left 12
        { 0.0f,  BLOCK_STEP,  BLOCK_STEP, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f,  BLOCK_STEP, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f,  BLOCK_STEP,  BLOCK_STEP, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,  BLOCK_STEP, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },

        // NOTE(matthias): Right 18
        {  BLOCK_STEP,  BLOCK_STEP,  BLOCK_STEP, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f,  BLOCK_STEP, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP,  BLOCK_STEP, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        // NOTE(matthias): Bottom 24
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f,  BLOCK_STEP, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f, 0.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP, 0.0f,  BLOCK_STEP, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,  BLOCK_STEP, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f },

        // NOTE(matthias): top 30
        { 0.0f,  BLOCK_STEP, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP, 0.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP,  BLOCK_STEP, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP,  BLOCK_STEP,  BLOCK_STEP, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        { 0.0f,  BLOCK_STEP,  BLOCK_STEP, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f,  BLOCK_STEP, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
	};

    static i32 UVs[] = {
        // Air
        0,0,0,0,0,0,
        // Grass
        // Bottom
         0, 15 ,
         // Side
         0, 14 ,
         // Top
         0, 13 ,
         // Stone
         // Bottom
         5, 15 ,
         // Side
         5, 15 ,
         // Top
         5, 15 ,
         // Dirt
         // Bottom
         0, 15,
         // Side
         0, 15,
         // Top
         0, 15

    };

    enum CubeDir
    {
        bottom = 0,
        side,
        top,
        cubeDirCount
    };

#define TEXTURE_STEP (1.0f / 16.0f)

    static void Get_UV(r32* u, r32* v, BlockType type, CubeDir dir)
    {
        i32* texLoc = UVs + (type*(3*2))+(dir*2);
        *u = *u == 0.0f ? (texLoc[0] * TEXTURE_STEP) : (texLoc[0] * TEXTURE_STEP) + TEXTURE_STEP;
        *v = *v == 0.0f ? (texLoc[1] * TEXTURE_STEP) : (texLoc[1] * TEXTURE_STEP) + TEXTURE_STEP;
    }

	void Add_Voxel_Face(Array<Vertex>* vertices, i32 faceIndex, vec3* vertexPos, BlockType blockType, CubeDir dir)
	{

		for (i32 vert = 0; vert < 6; vert++)
		{
			Vertex vertex = cube[vert + faceIndex];
			vertex.x += vertexPos->x;
			vertex.y += vertexPos->y;
			vertex.z += vertexPos->z;
            Get_UV(&vertex.u, &vertex.v, blockType, dir);

			vertices->Add(vertex);
		}
	}

    void Generate_Voxel_Chunk_Mesh(Voxel_World* world, i32 chunkX, i32 chunkY)
	{
        Voxel_Chunk* chunk = Voxel_Chunk_Read_Hash(chunkX, chunkY, (HashNode**)&world->ChunkHashMap);
        if(chunk == 0)
            return;
        Voxel_Chunk* LeftChunk = Voxel_Chunk_Read_Hash(chunkX-1, chunkY, (HashNode**)&world->ChunkHashMap);
        Voxel_Chunk* RightChunk = Voxel_Chunk_Read_Hash(chunkX + 1, chunkY, (HashNode**)&world->ChunkHashMap);
        Voxel_Chunk* FrontChunk = Voxel_Chunk_Read_Hash(chunkX, chunkY-1, (HashNode**)&world->ChunkHashMap);
        Voxel_Chunk* BackChunk = Voxel_Chunk_Read_Hash(chunkX, chunkY+1, (HashNode**)&world->ChunkHashMap);

		Array<Vertex> vertices;
        vec3 Vertex_Pos(0.0f, 0.0f, 0.0f);
		for (i32 level = 0; level < CHUNK_HEIGHT; level++)
		{
			for (i32 row = 0; row < CHUNK_WIDTH; row++)
			{
				for (i32 col = 0; col < CHUNK_WIDTH; col++)
				{
					if (chunk->Blocks[(level*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH) + col)].BlockType > Air)
					{
                        b8 IsOnRight = col == CHUNK_WIDTH - 1;
                        b8 IsOnLeft = col == 0;
                        b8 IsOnBottom = level == 0;
                        b8 IsOnTop = level == CHUNK_HEIGHT - 1;
                        b8 IsOnBack = row == CHUNK_WIDTH - 1;
                        b8 IsOnFront = row == 0;

                        b8 BlockLeftClear , BlockRightClear , BlockFrontClear, BlockBackClear;
                        BlockLeftClear = BlockRightClear = BlockFrontClear = BlockBackClear =false;

                        if(IsOnRight || IsOnLeft || IsOnRight || IsOnFront || IsOnBack)
                        {
                            BlockLeftClear = LeftChunk == 0 ? false : LeftChunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH) + (CHUNK_WIDTH - 1))].BlockType == Air;
                            BlockRightClear = RightChunk == 0 ? false : RightChunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH))].BlockType == Air;
                            BlockFrontClear = FrontChunk == 0 ? false : FrontChunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + (((CHUNK_WIDTH - 1)*CHUNK_WIDTH) + (col))].BlockType == Air;
                            BlockBackClear =  BackChunk == 0 ? false : BackChunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + col].BlockType == Air;
                        }

                        vec3 Voxel_Vertex_Pos(0.0f, 0.0f, 0.0f);
                        for(i32 vLevel=0; vLevel<BLOCK_WIDTH; vLevel++)
                        {
                            for(i32 vRow=0; vRow<BLOCK_WIDTH; vRow++)
                            {
                                for(i32 vCol=0; vCol<BLOCK_WIDTH; vCol++)
                                {
                                   Voxel_Block currentBlock = chunk->Blocks[(level*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH) + col)];
                                    if(currentBlock.BlockType > 0)
                                    {
                                        //printf("terp\n");
                                        vec3 Pos = Voxel_Vertex_Pos + Vertex_Pos;

                                        b8 IsVoxelOnRight = vCol == BLOCK_WIDTH - 1;
                                        b8 IsVoxelOnLeft = vCol == 0;
                                        b8 IsVoxelOnBottom = vLevel == 0;
                                        b8 IsVoxelOnTop = vLevel == BLOCK_WIDTH - 1;
                                        b8 IsVoxelOnBack = vRow == BLOCK_WIDTH - 1;
                                        b8 IsVoxelOnFront = vRow == 0;

                                        b8 VoxelAboveBlockClear = IsOnTop ? true : chunk->Blocks[((level + 1)*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH) + col)].BlockType == 0;
                                        b8 VoxelBelowBlockClear = IsOnBottom ? true : chunk->Blocks[((level - 1)*(CHUNK_WIDTH*CHUNK_WIDTH)) + (((row)*CHUNK_WIDTH) + (col))].BlockType == 0;
                                        b8 VoxelLeftBlockClear = IsOnLeft ? true : chunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH) + (col - 1))].BlockType == 0;
                                        b8 VoxelRightBlockClear = IsOnRight ? true : chunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + ((row*CHUNK_WIDTH) + (col + 1))].BlockType == 0;
                                        b8 VoxelFrontBlockClear = IsOnFront ? true : chunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + (((row - 1)*CHUNK_WIDTH) + (col))].BlockType == 0;
                                        b8 VoxelBackBlockClear = IsOnBack ? true : chunk->Blocks[((level)*(CHUNK_WIDTH*CHUNK_WIDTH)) + (((row + 1)*CHUNK_WIDTH) + (col))].BlockType == 0;

                                        b8 VoxelAboveClear = IsVoxelOnTop ? VoxelAboveBlockClear : currentBlock.BlockType == 0;
                                        b8 VoxelBelowClear = IsVoxelOnBottom ? VoxelBelowBlockClear :currentBlock.BlockType == 0;
                                        b8 VoxelLeftClear = IsVoxelOnLeft  ? VoxelLeftBlockClear : currentBlock.BlockType == 0;
                                        b8 VoxelRightClear = IsVoxelOnRight ? VoxelRightBlockClear : currentBlock.BlockType == 0;
                                        b8 VoxelFrontClear = IsVoxelOnFront ? VoxelFrontBlockClear : currentBlock.BlockType == 0;
                                        b8 VoxelBackClear = IsVoxelOnBack ? VoxelBackBlockClear : currentBlock.BlockType == 0;

                                        b8 SkinTop = false;//VoxelAboveClear && VoxelAboveBlockClear;
                                        b8 SkinBottom = false;//= VoxelBelowClear && VoxelBelowBlockClear;
                                        b8 SkinLeft = false;//VoxelLeftClear && VoxelLeftBlockClear;
                                        b8 SkinRight = false;//VoxelRightClear && VoxelRightBlockClear;
                                        b8 SkinBack = false;//VoxelBackClear && VoxelBackBlockClear;
                                        b8 SkinFront = false;//VoxelFrontClear && VoxelFrontBlockClear;



                                        SkinTop = VoxelAboveClear;
                                        SkinBottom = IsOnBottom && IsVoxelOnBottom ? false : VoxelBelowClear;
                                        SkinLeft = IsOnLeft && IsVoxelOnLeft ? BlockLeftClear : VoxelLeftClear;
                                        SkinRight = IsOnRight && IsVoxelOnRight ? BlockRightClear : VoxelRightClear;
                                        SkinBack = IsOnBack && IsVoxelOnBack ? BlockBackClear : VoxelBackClear;
                                        SkinFront = IsOnFront && IsVoxelOnFront ? BlockFrontClear : VoxelFrontClear;


                                        if (SkinTop)
                                        {
                                            Add_Voxel_Face(&vertices, 30, &Pos, currentBlock.BlockType, top);
                                        }
                                        if (SkinRight)
                                        {
                                            Add_Voxel_Face(&vertices, 18, &Pos, currentBlock.BlockType, side);
                                        }
                                        if (SkinLeft)
                                        {
                                            Add_Voxel_Face(&vertices, 12, &Pos, currentBlock.BlockType, side);
                                        }
                                        if (SkinBack)
                                        {
                                            Add_Voxel_Face(&vertices, 6, &Pos, currentBlock.BlockType, side);
                                        }
                                        if (SkinFront)
                                        {
                                            Add_Voxel_Face(&vertices, 0, &Pos, currentBlock.BlockType, side);
                                        }
                                        if (SkinBottom)
                                        {
                                            Add_Voxel_Face(&vertices, 24, &Pos, currentBlock.BlockType, bottom);
                                        }
                                    }
                                    Voxel_Vertex_Pos.x += BLOCK_STEP;
                                }
                                Voxel_Vertex_Pos.x = 0.0f;
                                Voxel_Vertex_Pos.z += BLOCK_STEP;
                            }
                            Voxel_Vertex_Pos.y += BLOCK_STEP;
                            Voxel_Vertex_Pos.z = 0.0f;
                        }
					}
					Vertex_Pos.x += BLOCK_METER;
				}
				Vertex_Pos.x = 0.0f;
				Vertex_Pos.z += BLOCK_METER;
			}
			Vertex_Pos.y += BLOCK_METER;
			Vertex_Pos.z = 0.0f;
		}

		Array<u32> indices;
		for (i32 i = 0; i < vertices.Size; i++) indices.Add(i);

		Mesh mesh = Gen_Mesh(vertices, indices);
		chunk->generate = false;

        if(chunk->MeshID == 0)
        {
            ASSETS::Meshes.Add(mesh);
            chunk->MeshID = ASSETS::Meshes.Size - 1;
        }
        else
        {
            ASSETS::Meshes[chunk->MeshID] = mesh;
        }

		return;
	}

	void Update_Voxel_World(Voxel_World* world, vec3 playerPos)
	{
		i32 chunkX = playerPos.x / (BLOCK_METER * CHUNK_WIDTH);
		i32 chunkY = playerPos.z / (BLOCK_METER * CHUNK_WIDTH);
        r32 radiusSquared = CHUNK_GEN_RADIUS*CHUNK_GEN_RADIUS;

		static bool start = true;
		if (start)
		{
			start = false;
			Voxel_Chunk_Write_To_Disk(chunkX, chunkY, world);
		}

        for(i32 y=-CHUNK_GEN_RADIUS; y < CHUNK_GEN_RADIUS; y++)
        {
            for(i32 x=-CHUNK_GEN_RADIUS; x < CHUNK_GEN_RADIUS; x++)
            {
                r32 lengthSquared = x*x + y*y;
                if(lengthSquared <= radiusSquared)
                {
                    i32 currentX = x + chunkX;
                    i32 currentY = y + chunkY;

                    Voxel_Chunk* chunk = Voxel_Chunk_Read_Hash(currentX, currentY, (HashNode**)&world->ChunkHashMap);
                    if(chunk == 0)
                    {
                        printf("Generating Chunk @ (%d, %d)\n", currentX, currentY);
                        Gen_Chunk(world, currentX, currentY);
                    }
                }
            }
        }
	}

    void Voxel_World_Gen_Chunk_Meshes(Voxel_World* world)
    {
        for (i32 chunk = 0; chunk < world->ChunkSize; chunk++)
        {
            Voxel_Chunk* CurrentChunk = &world->Chunks[chunk];
            if(CurrentChunk->generate)
            {

                i32 x = CurrentChunk->WorldPosX;
                i32 y = CurrentChunk->WorldPosY;
                Generate_Voxel_Chunk_Mesh(world, x, y);

                i32 dir[4][2] = {
                    {x+1, y  },
                    {x-1, y  },
                    {x  , y+1},
                    {x  , y-1}
                };

#if 1
                for(i32 i=0; i<4; i++)
                {
                    Generate_Voxel_Chunk_Mesh(world, dir[i][0], dir[i][1]);
                }
#endif
            }
        }
    }

	void Draw_Voxel_World(Queue_Array<RenderCommand>* rq, Voxel_World* voxelWorld) {
		i32 numChunks = voxelWorld->ChunkSize;

		for (i32 chunk = 0; chunk < numChunks; chunk++)
		{
			Voxel_Chunk* CurrentChunk = &voxelWorld->Chunks[chunk];
            if(CurrentChunk->MeshID)
            {
                RenderCommand rc = {};
                rc.RenderType = MESHRENDER;
                rc.ShaderType = SIMPLEMESH;
                rc.Mesh = CurrentChunk->MeshID;
                rc.Texture = ASSETS::TEXTURES::TileMap;
                rc.Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
                r32 gamePositionX = CurrentChunk->WorldPosX * (BLOCK_METER*CHUNK_WIDTH);
                r32 gamePositionY = CurrentChunk->WorldPosY * (BLOCK_METER*CHUNK_WIDTH);
                rc.Pos = vec3(gamePositionX, 0.0f, gamePositionY);
                rc.Scale = vec3(1.f, 1.f, 1.f);
                rc.Quat = quat::zero;//.from_angle_axis(toRadians(90.0f), vec3(1.0f, 0.0f, 0.0f));
                r32 rot = 0.0f;
                rq->Push(rc);
            }
            b8 debug = true;
            if(debug)
            {
                for(i32 i=0; i<4; i++)
                {
					r32 gamePositionX, gamePositionY;
                    RenderCommand rc = {};
                    rc.RenderType = SIMPLE3DDEBUGLINES;
                    rc.Color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
                    if(i < 2)
                    {
                        gamePositionX = CurrentChunk->WorldPosX * (BLOCK_METER*CHUNK_WIDTH +(BLOCK_METER*CHUNK_WIDTH*i));
                        gamePositionY = CurrentChunk->WorldPosY * (BLOCK_METER*CHUNK_WIDTH);
                    }
                    else
                    {
                        gamePositionX = CurrentChunk->WorldPosX * (BLOCK_METER*CHUNK_WIDTH +(BLOCK_METER*CHUNK_WIDTH*(i-2)));
                        gamePositionY = CurrentChunk->WorldPosY * (BLOCK_METER*CHUNK_WIDTH) +(BLOCK_METER*CHUNK_WIDTH);
                    }
                    rc.Point1 = vec3(gamePositionX, 0.0f, gamePositionY);
                    rc.Point2 = vec3(gamePositionX, 256.0f, gamePositionY);
                    rq->Push(rc);
                }
            }
		}
	}
}
