#include "voxel.hpp"
#include "assets.hpp"
#include "array.hpp"
#include "stdlib.h"

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

	void Remove_Block(Voxel_World* world, i32 chunkX, i32 chunkY, i32 blockX, i32 blockY, i32 blockZ)
	{
		Voxel_Chunk* chunk = Voxel_Chunk_Read_Hash(chunkX, chunkY, (HashNode**)&world->ChunkHashMap);

		i32 blockIndex = (blockZ * (CHUNK_WIDTH*CHUNK_WIDTH)) + (blockY * CHUNK_WIDTH) + blockX;
		chunk->Blocks[blockIndex].BlockType = Air;
	}

	void Dig_Caves(Voxel_World* world, i32 chunkX, i32 chunkY)
	{
		PerlinData p = Init_Perlin(94302831);
		r64 genStep = 0.06;

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
                                                 1, &p);
                    if(heightDouble < 0.25)
                    {
                        Remove_Block(world, chunkX, chunkY, x, y, z);
                    }
                }
            }
        }
	}

	Voxel_World* Generate_Voxel_World()
	{
		Voxel_World* result = (Voxel_World*)calloc(1, sizeof(Voxel_World));
		u32 numChunks = CHUNK_GEN_RADIUS * CHUNK_GEN_RADIUS;

		i32 currentChunkX = -(CHUNK_GEN_RADIUS/2);
		i32 currentChunkY = -(CHUNK_GEN_RADIUS/2);
		for (i32 chunk = 0; chunk < numChunks; chunk++)
		{
			u8* heightMap = Generate_Chunk_HeightMap(currentChunkX, currentChunkY);
			Generate_HeightMap_Voxel_Chunk(heightMap, &(result->Chunks[chunk]));
			//result->Chunks[chunk].ChunkID = chunk;
			result->Chunks[chunk].WorldPosX = currentChunkX;
			result->Chunks[chunk].WorldPosY = currentChunkY;
            result->ChunkSize++;
            Voxel_Chunk_Write_Hash(currentChunkX, currentChunkY, &result->Chunks[chunk], (HashNode**)&result->ChunkHashMap);
            Dig_Caves(result, currentChunkX, currentChunkY);
			currentChunkX++;
			if (currentChunkX == (CHUNK_GEN_RADIUS/2))
			{
				currentChunkX = -(CHUNK_GEN_RADIUS/2);
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

#if 0
	Voxel_Chunk* Generate_Debug_Voxel_Chunk()
	{
		Voxel_Chunk* result = (Voxel_Chunk*)calloc(1, sizeof(Voxel_Chunk));

		u32 level = CHUNK_VOLUME - (CHUNK_WIDTH*CHUNK_WIDTH)*(CHUNK_HEIGHT / 2);
		for (i32 i = 0; i < level; i++)
		{
			if (i % 2 == 0)
				result->Blocks[i] = 1;
		}

		return result;
	}
#endif

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

	void Generate_Voxel_Indices(Array<u32>* indices)
	{
		u32 Cube_Initial_Vertex = 0;
		u32 Vertex_Width = CHUNK_WIDTH + 1;
		for (i32 level = 0; level < CHUNK_HEIGHT; level++)
		{
			for (i32 row = 0; row < CHUNK_WIDTH; row++)
			{
				for (i32 col = 0; col < CHUNK_WIDTH; col++)
				{
					//__builtin_debugtrap();
					//printf("%d\n", Cube_Initial_Vertex);
					// NOTE(matthias): forward face
					u32 LLFIndex = Cube_Initial_Vertex;
					u32 LRFIndex = Cube_Initial_Vertex + 1;
					u32 URFIndex = Cube_Initial_Vertex + (Vertex_Width*Vertex_Width) + 1;
					u32 ULFIndex = Cube_Initial_Vertex + (Vertex_Width*Vertex_Width);
					u32 ULBIndex = Cube_Initial_Vertex + (Vertex_Width*Vertex_Width) + (Vertex_Width);
					u32 URBIndex = Cube_Initial_Vertex + (Vertex_Width*Vertex_Width) + Vertex_Width + 1;
					u32 LLBIndex = Cube_Initial_Vertex + Vertex_Width;
					u32 LRBIndex = Cube_Initial_Vertex + Vertex_Width + 1;

					{
						// NOTE(matthias): First Triangle
						// NOTE(matthias): lower left forward vertex
						indices->Add(LLFIndex);
						// NOTE(matthias): lower right forward vertex;
						indices->Add(LRFIndex);
						// NOTE(matthias): upper right forward vertex;
						indices->Add(URFIndex);

						// NOTE(matthias): Second Triangle
						// NOTE(matthias): lower left forward vertex
						indices->Add(LLFIndex);
						// NOTE(matthias): upper left forward vertex;
						indices->Add(ULFIndex);
						// NOTE(matthias): upper right forward vertex;
						indices->Add(URFIndex);
					}

					// NOTE(matthias): Left face
					{
						// NOTE(matthias): First Triangle
						// NOTE(matthias): lower left forward vertex
						indices->Add(LLFIndex);
						// NOTE(matthias): lower left back vertex;
						indices->Add(LLBIndex);
						// NOTE(matthias): upper left back vertex;
						indices->Add(ULBIndex);

						// NOTE(matthias): Second Triangle
						// NOTE(matthias): lower left forward vertex
						indices->Add(LLFIndex);
						// NOTE(matthias): upper left forward vertex;
						indices->Add(ULFIndex);
						// NOTE(matthias): lower left back vertex
						indices->Add(ULBIndex);
					}

					// NOTE(matthias): back face
					{
						// NOTE(matthias): First Triangle
						// NOTE(matthias): lower left back vertex
						indices->Add(LLBIndex);
						// NOTE(matthias): lower right back vertex;
						indices->Add(LRBIndex);
						// NOTE(matthias): upper right back vertex;
						indices->Add(URBIndex);

						// NOTE(matthias): Second Triangle
						// NOTE(matthias): lower left back vertex
						indices->Add(LLBIndex);
						// NOTE(matthias): upper left back vertex;
						indices->Add(ULBIndex);
						// NOTE(matthias): upper right back vertex;
						indices->Add(URBIndex);
					}

					// NOTE(matthias): right face
					{
						// NOTE(matthias): First Triangle
						// NOTE(matthias): lower right back vertex
						indices->Add(LRBIndex);
						// NOTE(matthias): lower right forward vertex;
						indices->Add(LRFIndex);
						// NOTE(matthias): upper right forward vertex;
						indices->Add(URFIndex);

						// NOTE(matthias): Second Triangle
						// NOTE(matthias): lower right back vertex
						indices->Add(LRBIndex);
						// NOTE(matthias): upper right back vertex;
						indices->Add(URBIndex);
						// NOTE(matthias): upper right forward vertex;
						indices->Add(URFIndex);
					}

					// NOTE(matthias): top face
					{
						// NOTE(matthias): First Triangle
						// NOTE(matthias): upper left forward vertex
						indices->Add(ULFIndex);
						// NOTE(matthias): upper left back vertex;
						indices->Add(ULBIndex);
						// NOTE(matthias): upper right back vertex;
						indices->Add(URBIndex);

						// NOTE(matthias): Second Triangle
						// NOTE(matthias): upper left forward vertex
						indices->Add(ULFIndex);
						// NOTE(matthias): upper right forward vertex;
						indices->Add(URFIndex);
						// NOTE(matthias): upper right back vertex;
						indices->Add(URBIndex);
					}

					// NOTE(matthias): bottom face
					{
						// NOTE(matthias): First Triangle
						// NOTE(matthias): lower left forward vertex
						indices->Add(LLFIndex);
						// NOTE(matthias): lower lefer back vertex;
						indices->Add(LLBIndex);
						// NOTE(matthias): lower right back vertex;
						indices->Add(LRBIndex);

						// NOTE(matthias): Second Triangle
						// NOTE(matthias): lower left forward vertex
						indices->Add(LLFIndex);
						// NOTE(matthias): lower rigth forward vertex;
						indices->Add(LRFIndex);
						// NOTE(matthias): lower right forward vertex;
						indices->Add(LRBIndex);
					}

					Cube_Initial_Vertex += 1;
				}
				Cube_Initial_Vertex += 1;//row == CHUNK_WIDTH-1 ? 2 : 1;
			}
			Cube_Initial_Vertex += Vertex_Width;
		}
	}
//Front Right Top
	static Vertex cube[] = {
        // NOTE(matthias): Front 0
		{ -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
        // NOTE(matthias): Back 6
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f },

        // NOTE(matthias): Left 12
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f },

        // NOTE(matthias): Right 18
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        // NOTE(matthias): Bottom 24
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f },

        // NOTE(matthias): top 30
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        {  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f },
        { -BLOCK_STEP/2.0f,  BLOCK_STEP/2.0f, -BLOCK_STEP/2.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
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

	Mesh* Generate_Voxel_Chunk_Mesh(Voxel_World* world, i32 chunkX, i32 chunkY)
	{
        Voxel_Chunk* chunk = Voxel_Chunk_Read_Hash(chunkX, chunkY, (HashNode**)&world->ChunkHashMap);
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

		Mesh* mesh = new Mesh;
		mesh->indices = indices;
		mesh->vertices = vertices;
		mesh->GenVertexObjects();
		Material mat = {};
		mat.ambient = vec3(0.135f, 0.2225f, 0.1575f);
		mat.diffuse = vec3(0.54f, 0.89f, 0.63f);
		mat.specular = vec3(0.316228f, 0.316228f, 0.316228f);
		mat.shininess = 0.1f;
		mesh->material = mat;

		return mesh;
	}

	void Update_Voxel_World(Voxel_World* world, vec3 playerPos)
	{
		i32 chunkX = playerPos.x / (BLOCK_METER * CHUNK_WIDTH);
		i32 chunkY = playerPos.z / (BLOCK_METER * CHUNK_WIDTH);
		printf("CHUNK: x: %d, y: %d\n", chunkX, chunkY);
	}

	void Draw_Voxel_World(Queue_Array<RenderCommand>* rq, Voxel_World* voxelWorld) {
		i32 numChunks = voxelWorld->ChunkSize;

		for (i32 chunk = 0; chunk < numChunks; chunk++)
		{
			Voxel_Chunk* CurrentChunk = &voxelWorld->Chunks[chunk];
			RenderCommand rc = {};
			rc.RenderType = MESHRENDER;
			rc.ShaderType = SIMPLEMESH;
			rc.Mesh = CurrentChunk->MeshID;
			rc.Model = 0;
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
	}
}
