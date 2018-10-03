#pragma once

#include "game.hpp"
#include "sMath.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shared.hpp"

namespace SMOBA
{

#define CHUNK_MAX 256
#define CHUNK_MAX_ACTIVE 32;
#define CHUNK_GEN_RADIUS 4
#define CHUNK_GEN_DIAMETER CHUNK_GEN_RADIUS * 2
#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 256
#define CHUNK_VOLUME CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT
#define BLOCK_METER 1.0f
#define BLOCK_WIDTH 1
#define BLOCK_VOLUME BLOCK_WIDTH*BLOCK_WIDTH*BLOCK_WIDTH
#define BLOCK_STEP BLOCK_METER / BLOCK_WIDTH
#define GROUND_MIN 56
#define GROUND_MAX 96


    enum BlockType : u16
    {
        Air,
        Grass,
        Stone,
        Dirt,
        BlockTypeCount
    };

    const u32 CHUNK_MAGIC = 'BITV';
    #define CHUNK_FILE_INDEX_COUNT 1024

#pragma pack(push, 1)
    struct Chunk_File_Header
    {
        const u32 Magic = CHUNK_MAGIC;
        u64 Size; //NOTE(matthias): Size of entire file.
        u64 IndexOffset; //NOTE(matthias): File index location starting from the end of the header.
    };

    struct Chunk_Data_Header
    {

    };

    struct Chunk_File_Index
    {
        u16 NumEntries;
        struct Entry
        {
            u64 Offset;
            i32 X, Y;
        }Entries[CHUNK_FILE_INDEX_COUNT];
    };
#pragma pack(pop)

    struct Voxel_Block
    {
        BlockType BlockType;
        //u16 Voxels[BLOCK_VOLUME];
    };

    struct Voxel_Chunk
    {
        b8 generate;
		b8 Active;
        ID MeshID;
        i32 WorldPosX;
        i32 WorldPosY;
        Voxel_Block Blocks[CHUNK_VOLUME];
    };

    struct Voxel_Frozen_Chunk
    {
		u32 Size;
        u8* Data;
    };

    struct Voxel_World
    {
        u64 Seed;
        //Voxel_Chunk* ActiveChunks[CHUNK_MAX_ACTIVE];
        i32 ChunkSize;
        Voxel_Chunk Chunks[CHUNK_MAX];
        HashNode* ChunkHashMap[HASH_TABLE_SIZE];
    };

    Voxel_World* Generate_Voxel_World();
    u8* Generate_Chunk_HeightMap(i32 chunkPosX, i32 chunkPosY);
    //Voxel_Chunk* Generate_Debug_Voxel_Chunk();
    Voxel_Chunk* Generate_HeightMap_Voxel_Chunk(u8* heightMap, Voxel_Chunk* chunk);
	void Generate_Voxel_Chunk_Mesh(Voxel_World* world, i32 chunkX, i32 chunkY);
	void Update_Voxel_World(Voxel_World* world, vec3 playerPos);
    void Voxel_World_Gen_Chunk_Meshes(Voxel_World* world);
    void Draw_Voxel_World(Queue_Array<RenderCommand>* rq, Voxel_World* voxelWorld);
}
