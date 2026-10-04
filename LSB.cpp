#include <intrin.h>
#include <cassert>
#include "LSB.h"

#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse)

unsigned int LSB(U64 mask)
{
	unsigned long index;
	unsigned char success{ _BitScanForward64(&index, mask) };
	success;
	assert( success && "bitscan called on 0 mask, no set bits found");
	return index;
}

unsigned int MSB(U64 mask)
{
	unsigned long index;
	unsigned char success{ _BitScanReverse64(&index, mask) };
	success;
	assert(success && "bitscan call on 0 mask, no set bits found");
	return index;
}