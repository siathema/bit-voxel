#pragma once

#include <string.h>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include "game.hpp"
#include "sAssert.hpp"

namespace SMOBA
{

#define DEFAULT_ARRAY_SIZE 1
	template<typename T, u16 size>
	struct StaticArray
	{
		union {
			T* Data;
			u8* _data;
		};
		i32 DataSize;

		StaticArray() {
			DataSize = DEFAULT_ARRAY_SIZE;
			Data = (T*)calloc(size, sizeof(T));
		}

		~StaticArray() { free(Data); }

		const T& operator[](i32 i) const {
			s_assert(i >= 0 && i < size, "Index out of Bounds!");
			return *(Data + i);
		}

		T& operator[](i32 i) {
			s_assert(i >= 0 && i < size, "Index out of Bounds!");
			return *(Data + i);
		}

		void Resize(i32 s) {
			s_assert(s > DataSize && s < size);
			DataSize = s;
		}

		void Remove(i32 i) {
			s_assert(i >= 0 && i < DataSize);
			if (i != (DataSize - 1)) {
				Data[i] = Data[DataSize - 1];
			}
			DataSize--;
		}

		void Remove_Ordered(i32 i) {
			s_assert(i >= 0 && i < DataSize);
			if (i != (DataSize - 1)) {
				memmove((Data + i), (Data + i + 1), (DataSize - (i - 1))*sizeof(T));
			}
			DataSize--;
		}

		T& Add() {
			Resize(DataSize+1);
			return Data[DataSize - 1];
		}

		T& Add(T item) {
			Add();
			return Data[DataSize - 1] = item;
		}

		T& Insert(i32 i) {
			Resize(DataSize+1);
			memmove((Data + i + 1), (Data + i), (DataSize - i)*sizeof(T));
			return Data[i];
		}

		T& Insert(i32 i, T item) {
			Insert(i);
			return Data[i] = item;
		}
	};

	template<typename T>
	struct Array
	{
		union {
			T* Data;
			u8* _data;
		};
		i32 Size;
		i32 Reserve;

		Array() {
			Reserve = 1;
			Size = 0;
			Data = (T*)calloc(Reserve, sizeof(T));
		}

		~Array() {}// if(Data) free(Data); }

		const T& operator[](i32 i) const {
			s_assert(i >= 0 && i < Size);
			return *(Data + i);
		}

		T& operator[](i32 i) {
			s_assert(i >= 0 && i < Size, "Index out of range\n");
			return *(Data + i);
		}

		/*T& operator[](i32 i) volatile {
			s_assert(i >= 0 && i < size);
			return *(Data + i);
		}*/

		void Reserve_Mem(i32 s) {
			s_assert(s > Reserve, "s is too smale\n");
			u32 newSize = (u32)pow(1.5, (i32)(log(s) / log(1.5)) + 1);

			Data = (T*)realloc(Data, sizeof(T) * newSize);
			memset((Data + Size), 0, newSize - Size);
			Reserve = newSize;
		}

		/*void reserve_mem(i32 s) volatile {
			s_assert(s > reserve);
			u32 newSize = (u32)pow(1.5, (i32)(log(s) / log(1.5)) + 1);

			Data = (T*)realloc(Data, sizeof(T) * newSize);
			memset((Data + size), 0, newSize - size);
			reserve = newSize;
		}*/

		void Resize(i32 s) {
			s_assert(s > Size, "s is too small\n");

			if (s <= Reserve) {
				Size = s;
			}
			else {
				Reserve_Mem(s);
				Size = s;

			}
		}

		/*void Resize(i32 s) volatile {
			s_assert(s > size);

			if (s <= reserve) {
				size = s;
			}
			else {
				reserve_mem(s);
				size = s;

			}
		}*/

		void Remove(i32 i) {
			s_assert(i >= 0 && i < Size, "Indexe out of range!\n");
			if (i != (Size - 1)) {
				Data[i] = Data[Size - 1];
			}
			Size--;
		}

		void Remove_Ordered(i32 i) {
			s_assert(i >= 0 && i < Size, "Index out of range!\n");
			if (i != (Size - 1) || (Size != 1)) {
				memmove((Data + i), (Data + i + 1), (Size - (i+1))*sizeof(T));
			}
			Size--;
		}

		T& Add() {
			Resize(Size+1);
			return Data[Size - 1];
		}

		/*T& Add() volatile {
			Resize(size + 1);
			return Data[size - 1];
		}*/

		T& Add(T item) {
			Add();
			return Data[Size - 1] = item;
		}

		/*T& Add(T item) volatile {
			Add();
			return Data[size - 1] = item;
		}*/

		T& Insert(i32 i) {
			Resize(Size+1);
			memmove((Data + i + 1), (Data + i), (Size - i)*sizeof(T));
			return Data[i];
		}

		T& Insert(i32 i, T item) {
			Insert(i);
			return Data[i] = item;
		}

	};

	template<typename T>
	struct Queue_Array : public Array<T>
	{
		Queue_Array() : Array<T>() {}

		void Push(T item)
		{
			this->Add(item);
		}

		T Pop()
		{
			if (this->Size == 0) s_assert(false, "Queue Empty!");

			T result = this->Data[0];
			this->Remove_Ordered(0);
			return result;
		}
	};

	template<typename T>
	void test(const T& a, const T& b) {
		if (a == b) {
			printf("Passed\n");
		}
		else {
			printf("Failed\n");
		}
	}

#ifdef TEST
	void testArray() {
		struct foo {
			i32 x;
			r32 y;
			char* s;
		};

		Array<i32> intArray;
		Array<foo> fooArray;

		intArray.Add(65);
		intArray.Resize(100);
		for (i32 i = 1; i < intArray.Size; i++) {
			intArray[i] = i;
		}

		for (i32 i = 0; i < intArray.Size; i++) {
			printf("%d, ", intArray[i]);
		}
		printf("\n");
		printf("size = %d\n", intArray.Size); // = 100
		printf("reserve size = %d\n", intArray.Reserve); // = 150

		intArray.Insert(40, 3980);
		intArray.Insert(intArray.Size - 1, 54023);
		intArray.Remove(20);
		intArray.Remove(30);
		intArray.Remove_Ordered(2);
		intArray.Remove_Ordered(45);

		for (i32 i = 0; i < intArray.Size; i++) {
			printf("%d, ", intArray[i]);
		}
		printf("\n");
		printf("size = %d\n", intArray.Size);
		printf("reserve size = %d\n", intArray.Reserve);

		printf("Reserve size = 150 ..."); test<i32>(150, intArray.Reserve);
		printf("size = 98 ..."); test<i32>(98, intArray.Size);
		printf("intArray[0] = 65 ..."); test<i32>(65, intArray[0]);
		printf("intArray[40] = 3980 ..."); test<i32>(3980, intArray[39]);
		printf("intArray[30] = 54023 ..."); test<i32>(54023, intArray[29]);
		printf("intArray[size-1] = 98 ..."); test<i32>(98, intArray[intArray.size-1]);

		//fooArray
		foo a = { 13, 1.0f,"hello" };
		fooArray.Add(a);
		fooArray.Resize(100);
		for (i32 i = 1; i < fooArray.Size; i++) {
			fooArray[i] = a;
		}

		for (i32 i = 0; i < fooArray.Size; i++) {
			printf("%d, ", fooArray[i].x);
			printf("%f, ", fooArray[i].y);
			printf("%s,\n", fooArray[i].s);
		}
		printf("\n");
		printf("size = %d\n", fooArray.Size); // = 100
		printf("reserve size = %d\n", fooArray.Reserve); // = 150

		foo x = { 593, 124.04726f, "burp" };
		foo y = { 4321, PI, "3.141591" };
		fooArray.Insert(40, x);
		fooArray.Insert(fooArray.size - 1, y);
		fooArray.Remove(20);
		fooArray.Remove(30);
		fooArray.Remove_Ordered(2);
		fooArray.Remove_Ordered(45);
		i32 oldSize = fooArray.Size;
		fooArray.Resize(1000);

		for (i32 i = oldSize; i < fooArray.Size; i++)
		{
			fooArray[i] = a;
		}

		for (i32 i = 0; i < fooArray.Size; i++) {
			printf("%d, ", fooArray[i].x);
			printf("%f, ", fooArray[i].y);
			printf("%s,\n", fooArray[i].s);
		}
		printf("\n");
		printf("size = %d\n", fooArray.Size);
		printf("reserve size = %d\n", fooArray.Reserve);

		printf("Reserve size = 150 ..."); test<i32>(150, fooArray.Reserve);
		printf("size = 98 ..."); test<i32>(98, fooArray.Size);
		printf("fooArray[0].x = 13 ..."); test<i32>(13, fooArray[0].x);
		printf("fooArray[0].y = 1.0 ..."); test<r32>(1.0f, fooArray[0].y);
		printf("fooArray[39].x = 593 ..."); test<i32>(593, fooArray[39].x);
		printf("fooArray[39].x = 124.04726 ..."); test<r32>(124.04726f, fooArray[39].y);

		printf("fooArray[29].x = 4321 ..."); test<i32>(4321, fooArray[29].x);
		printf("fooArray[29].y = %f ...", PI); test<r32>(PI, fooArray[29].y);

		printf("fooArray[size-1].x = 13 ..."); test<i32>(13, fooArray[fooArray.Size - 1].x);
		printf("fooArray[size-1].y = 1.0 ..."); test<i32>(1.0f, fooArray[fooArray.Size - 1].y);
	}
#endif
}
