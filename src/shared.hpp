#include "game.hpp"

namespace SMOBA
{
#define PERLIN_MAX 512
#define HASH_TABLE_SIZE 1024

    struct PerlinData
    {
        i32 p[PERLIN_MAX];
    };

    struct HashNode
    {
        HashNode* Prev;
        void* Address;
        u8* Key;
        i32 KeySize;
        HashNode* Next;
    };


    u32 ROL_32(u32 n, u32 r);
    u32 ROR_32(u32 n, u32 r);
    void HashStore( u8* key, u32 size, void* dataAddress, HashNode** table);
    void* HashRead(u8* key, u32 size, HashNode** table);
    void HashDelete(u8* key, u32 size, HashNode** table);

    void Shuffle_Int(i32* data, i32 size);
    PerlinData Init_Perlin(u32 seed);
    r64 Perlin_Noise(r64 x, r64 y, r64 z, PerlinData* p);
    r64 Perlin_Noise(r64 x, r64 y, PerlinData* p);
    r64 Octave_Noise(r64 x, r64 y, i32 octaves, PerlinData* p);
    r64 Octave_Noise(r64 x, r64 y, r64 z, i32 octaves, PerlinData* p);
    r64 Octave_Noise_Zero_To_One(r64 x, r64 y, i32 octaves, PerlinData* p);
    r64 Octave_Noise_Zero_To_One(r64 x, r64 y, r64 z, i32 octaves, PerlinData* p);

}
