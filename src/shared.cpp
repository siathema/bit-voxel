#include "shared.hpp"
#include "game.hpp"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

//NOTE(matthias): Based on https://github.com/Reputeless/PerlinNoise

namespace SMOBA
{

    u32 ROL_32(u32 n, u32 r)
    {
        u32 result = (n << r) | (n >> 32-r);
        return result;
    }

    u32 ROR_32(u32 n, u32 r)
    {
        u32 result = (n >> r) | (n << 32-r);
        return result;
    }

    //NOTE(matthias): based on https://en.wikipedia.org/wiki/MurmurHash
    u32 Murmur3_Hash_U32(u8* key, u32 size, u32 seed=0xDEADBEEF)
    {
        u32 result;
        u32 c1 = 0xCC9E2D51;
        u32 c2 = 0x1B873593;
        u32 r1 = 15;
        u32 r2 = 13;
        u32 m = 5;
        u32 n = 0xE6546B64;

        result = seed;

        u32* keyInt = (u32*)key;
        u32 numInt = size/4;
        u32 remainder = size % 4;
        for(u32 i=0; i<numInt; i++)
        {
            u32 k = keyInt[i];

            k *= c1;
            k = ROL_32(k, r1);
            k *= c2;

            result ^= k;
            result = ROL_32(result, r2);
            result = result * m + n;
        }
        // NOTE(matthias): Does not work accuratly on Big-Endian machines
        if(remainder)
        {
            u32 last = 0;
            for(u32 i=0; i<remainder; i++)
            {
                last |= key[(size - remainder) + i];
                if(i != (remainder - 1))
                    last <<= 8;
            }
            result ^= last;
        }

        result ^= size;
        result ^= (result >> 16);
        result *= 0x85EBCA6B;
        result ^= (result >> 13);
        result *= 0xC2B2AE35;
        result ^= (result >> 16);

        return result;
    }

    void HashStore( u8* key, u32 size, void* dataAddress, HashNode** table)
    {
        u32 hashIndex = Murmur3_Hash_U32(key, size);
        hashIndex %= HASH_TABLE_SIZE;
        HashNode* currentNode = table[hashIndex];
		if(currentNode == 0)
		{
			currentNode = (HashNode*)calloc(1, sizeof(HashNode));
			currentNode->Next = 0;
			currentNode->Address = dataAddress;

			currentNode->Key = (u8*)malloc(size);
			currentNode->KeySize = size;
			memcpy(currentNode->Key, key, size);
			table[hashIndex] = currentNode;

			return;
		}
        while(currentNode->Next != 0)
        {
			currentNode = currentNode->Next;
        }
		currentNode->Next = (HashNode*)calloc(1, sizeof(HashNode));
		currentNode->Next->Prev = currentNode;
		currentNode->Next->Address = dataAddress;

		currentNode->Next->Key = (u8*)malloc(size);
		currentNode->Next->KeySize = size;
		memcpy(currentNode->Next->Key, key, size);
    }

    void* HashRead(u8* key, u32 size, HashNode** table)
    {
        u32 hashIndex = Murmur3_Hash_U32(key, size);
        hashIndex %= HASH_TABLE_SIZE;
        HashNode* currentNode = table[hashIndex];
        if(currentNode == 0)
        {
            return 0;
        }
        do
        {
            if(currentNode->KeySize == size)
            {
                if(memcmp(key, currentNode->Key, size) == 0)
                {
                    return currentNode->Address;
                }
            }
            currentNode = currentNode->Next;
        } while(currentNode != 0);

        return 0;
    }

    void HashDelete(u8* key, u32 size, HashNode** table)
    {
        u32 hashIndex = Murmur3_Hash_U32(key, size);
        hashIndex %= HASH_TABLE_SIZE;
        HashNode* currentNode = table[hashIndex];
        if(currentNode == 0)
        {
            return;
        }
        do
        {
            if(currentNode->KeySize == size)
            {
                if(memcmp(key, currentNode->Key, size) == 0)
                {
                    currentNode->Prev->Next = currentNode->Next;
                    free(currentNode->Key);
                    free(currentNode);
                    return;
                }
            }
            currentNode = currentNode->Next;
        } while(currentNode != 0);

        return;
    }

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
        r64 result = t * t * t * (t * (t * 6 - 15) + 10);
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
		i32 BB = p->p[B + 1] + Z;

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
