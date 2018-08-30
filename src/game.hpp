#pragma once

#include <stdint.h>
/*
    NOTE(matthias): The format and style of this source is as follows:
	Tab width is four spaces. Scope-braces start on a new-line. Line-width should
	not excede 80 characters. If fuction declarations, function definitions,
	function calls, if-statments, while-statments, or for-statments go past 80
	characters, each item between the parentheses should be put on a new-line.

	Style-wise, all namespaces and #defines should be all caps. Structs, Classes,
	Enums, user types ,functions and methods should be Upper_snake_cased. Member
	and Global variables should beUpperCamelCased. local variables should be
	camelCased.
*/

#define CLAMP(x, min, max) ((x<min)||(x>max))?((x<min)? (min) : (max)):(x)
#define ARRAYCOUNT(a) ((sizeof(a))/(sizeof(a[0])))

namespace SMOBA
{
	typedef uint8_t u8;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint64_t u64;

	typedef int8_t i8;
	typedef int16_t i16;
	typedef int32_t i32;
	typedef int64_t i64;

	typedef float r32;
	typedef double r64;

	typedef u8 b8;
	typedef u16 ID;

	struct ViewportInfo
	{
		u32 ScreenHeight;
		u32 ScreenWidth;
		b8 Vsync;
	};

	struct Input
	{
		bool Keys[128];
		bool Up, Down, Left, Right;
		bool LeftMouseDown, RightMouseDown;
		i32 MouseX, MouseY, WheelX, WheelY;
	};
}
