#include "shared.hpp"
#include "game.hpp"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

//NOTE(matthias): Based on https://github.com/Reputeless/PerlinNoise

namespace SMOBA
{

    // implementation

    void Shuffle_Int(i32* data, i32 size)
    {
        i32* pTemp = (i32*)malloc(sizeof(i32) * size);
        memcpy(pTemp, data, sizeof(i32) * size);

        for(size_t i=0; i<size; i++)
        {
            size_t randomIndex = rand()% size;

            while(pTemp[randomIndex] == -1)
            {
                ++randomIndex;
                if(randomIndex == size)
                {
                    randomIndex = 0;
                }
            }
            data[i] = pTemp[randomIndex];
        }

        free(pTemp);
    }

    PerlinData Init_Perlin(u32 seed)
    {
        if(seed == 0)
        {
            srand(time(0));
        }
        else
        {
            srand(seed);
        }

        PerlinData result;
        i32 size = PERLIN_MAX/2;
        for(size_t i=0; i<size; i++)
        {
            result.p[i] = i;
        }

        Shuffle_Int(result.p, size);

        for(size_t i=0; i<size; i++)
        {
            result.p[i+size] = result.p[i];
        }

        return result;
    }

    static r64 Fade(r64 t)
    {
        r64 result = t * t * t * (t * (t * 6 - 15) +10);
        return result;
    }

    static r64 Lerp(r64 t, r64 a, r64 b)
    {
        r64 result = (a + (t * (b - a)));
        return result;
    }

    static r64 Grad(i32 hash, r64 x, r64 y, r64 z)
    {
        i32 h = hash & 15;
        r64 u = h < 8 ? x : y;
        r64 v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        r64 result = ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        return result;
    }

    r64 Perlin_Noise(r64 x, r64 y, r64 z, PerlinData* p)
    {
        i32 X = i32(floor(x)) & (i32)255;
        i32 Y = i32(floor(y)) & (i32)255;
        i32 Z = i32(floor(z)) & (i32)255;

        x -= floor(x);
        y -= floor(y);
        z -= floor(z);

        r64 u = Fade(x);
        r64 v = Fade(y);
        r64 w = Fade(z);

		i32 A = p->p[X] + Y;
		i32 AA = p->p[A] + Z;
		i32 AB = p->p[A + 1] + Z;
		i32 B = p->p[X + 1] + Y;
		i32 BA = p->p[B] + Z;
		i32 BB = p->p[B + 1] + z;

        r64 result =  Lerp(w, Lerp(v, Lerp(u, Grad(p->p[AA], x, y, z),
                                           Grad(p->p[BA], x - 1, y, z)),
                                   Lerp(u, Grad(p->p[AB], x, y - 1, z),
                                        Grad(p->p[BB], x - 1, y - 1, z))),
                           Lerp(v, Lerp(u, Grad(p->p[AA + 1], x, y, z - 1),
                                        Grad(p->p[BA + 1], x - 1, y, z - 1)),
                                Lerp(u, Grad(p->p[AB + 1], x, y - 1, z - 1),
                                     Grad(p->p[BB + 1], x - 1, y - 1, z - 1))));
        return result;
    }

    r64 Perlin_Noise(r64 x, r64 y, PerlinData* p)
    {
        r64 result = Perlin_Noise(x, y, 0.0, p);
        return result;
    }

    r64 Perlin_Noise(r64 x, PerlinData* p)
    {
        r64 result = Perlin_Noise(x, 0.0, 0.0, p);
        return result;
    }

    r64 Octave_Noise(r64 x, r64 y, i32 octaves, PerlinData* p)
    {
        r64 result = 0.0;
        r64 amp = 1.0;

        for(i32 i = 0; i < octaves; i++)
        {
            result += Perlin_Noise(x, y, p) * amp;
            x *= 2.0;
            y *= 2.0;
            amp *= 0.5;
        }

        return result;
    }

    r64 Octave_Noise(r64 x, r64 y, r64 z, i32 octaves, PerlinData* p)
    {
        r64 result = 0.0;
        r64 amp = 1.0;

        for(i32 i = 0; i < octaves; i++)
        {
            result += Perlin_Noise(x, y, z, p) * amp;
            x *= 2.0;
            y *= 2.0;
            z *= 2.0;
            amp *= 0.5;
        }

        return result;
    }

    r64 Octave_Noise_Zero_To_One(r64 x, r64 y, i32 octaves, PerlinData* p)
    {
        r64 result = Octave_Noise(x, y, octaves, p) * 0.5 + 0.5;
        return result;
    }

    r64 Octave_Noise_Zero_To_One(r64 x, r64 y, r64 z, i32 octaves, PerlinData* p)
    {
        r64 result = Octave_Noise(x, y, z, octaves, p) * 0.5 + 0.5;
        return result;
    }

}
