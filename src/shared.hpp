#include "game.hpp"

namespace SMOBA
{
#define PERLIN_MAX 512

    struct PerlinData
    {
        i32 p[PERLIN_MAX];
    };

    void Shuffle_Int(i32* data, size_t size);
    PerlinData Init_Perlin(u32 seed);
    r64 Perlin_Noise(r64 x, r64 y, r64 z, PerlinData* p);
    r64 Perlin_Noise(r64 x, r64 y, PerlinData* p);
    r64 Octave_Noise(r64 x, r64 y, i32 octaves, PerlinData* p);
    r64 Octave_Noise(r64 x, r64 y, r64 z, i32 octaves, PerlinData* p);
    r64 Octave_Noise_Zero_To_One(r64 x, r64 y, i32 octaves, PerlinData* p);
    r64 Octave_Noise_Zero_To_One(r64 x, r64 y, r64 z, i32 octaves, PerlinData* p);

}
