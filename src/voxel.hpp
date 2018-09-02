#pragma once

#include "game.hpp"
#include "sMath.hpp"
#include "mesh.hpp"
#include "renderer.hpp"

namespace SMOBA
{

#define CHUNK_MAX 4
#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 256
#define CHUNK_VOLUME CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT
#define BLOCK_METER 1.0f
#define BLOCK_WIDTH 1
#define BLOCK_VOLUME BLOCK_WIDTH*BLOCK_WIDTH*BLOCK_WIDTH
#define BLOCK_STEP BLOCK_METER / BLOCK_WIDTH
#define GROUND_MIN 56
#define GROUND_MAX 96

    struct Voxel_Block
    {
        ID BlockId;
        u16 Voxels[BLOCK_VOLUME];
    };

    struct Voxel_Chunk
    {
        ID ChunkID;
        ID MeshID;
        i32 WorldPosX;
        i32 WorldPosY;
        Voxel_Block Blocks[CHUNK_VOLUME];
    };

    struct Voxel_World
    {
        u64 Seed;
        Array<ID> GeneratedChunks;
        Voxel_Chunk Chunks[CHUNK_MAX*CHUNK_MAX];
    };

    Voxel_World* Generate_Voxel_World();
    u8* Generate_Chunk_HeightMap(i32 chunkPosX, i32 chunkPosY);
    //Voxel_Chunk* Generate_Debug_Voxel_Chunk();
    Voxel_Chunk* Generate_HeightMap_Voxel_Chunk(u8* heightMap, Voxel_Chunk* chunk);
    Mesh* Generate_Voxel_Chunk_Mesh(Voxel_World* world, ID chunkID);
    void Draw_Voxel_World(Queue_Array<RenderCommand>* rq, Voxel_World* voxelWorld);
}
