#if (__clang__)
#include <xmmintrin.h>
#endif

#include "game.hpp"
#include "sMath.hpp"

namespace SMOBA
{
  r32 sqrtf(r32 n)
  {
    __m128 reg;
    _mm_store_ss(&n, reg);
    reg = _mm_sqrt_ss(reg);
    return (r32)reg[0];
  }
}
