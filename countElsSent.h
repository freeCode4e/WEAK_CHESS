#include "../../HEADERS/ASSERT/ASSERT.h"

//template <typename START_PTR, typename END_TOKEN, typename INT_TYPE>
//INT_TYPE countElsSent(const START_PTR* start, const END_TOKEN* end)
//{
//	INT_TYPE count{ 0 };
//	while (start < end) {
//		++count;
//		++start;
//	}
//	ASSERT(start == end, "sentinel value not found!");
//	return count;
//}
//using INT_TYPE = int;

template <typename INT_TYPE, typename START_PTR, typename END_TOKEN>
INT_TYPE countElsSent(const START_PTR* start, const END_TOKEN end)
{
	ASSERT(start, "start pointer is null!");
	INT_TYPE count{ 0 };
	while (*start != end) {
		++count;
		++start;
	}
	return count;
}

template <typename T, typename INT_TYPE, INT_TYPE size>
constexpr INT_TYPE arraySize(T(&ar)[size])
{
	ar;
	return size;
}