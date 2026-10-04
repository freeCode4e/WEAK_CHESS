#pragma once
#include "types.h"
#include "LSB.h"
inline U64 get_bit(U64 b, U32 index) { return (b) & (1ULL << index); }
inline void set_bit(U64& b, U32 index) { (b) |= (1ULL << index); }

inline void set_bit(U32& b, U32 index) { (b) |= (1UL << index); }
inline void clear_bit(U64& b, U32 index) { (b) &= ~(1ULL << index); }
inline void clear_bit(U32& b, U32 index) { (b) &= ~(1UL << index); }
inline U32 pop_LSB(U64& b)
{
	U32 LSB_index{ LSB(b) };
	//clear_bit(b, LSB_index);
	b &= (b - 1);
	return LSB_index;
}
inline U32 pop_MSB(U64& b)
{
	U32 MSB_index{ MSB(b) };
	clear_bit(b, MSB_index);
	//b &= ((~0UL) >> 1);
	return MSB_index;
}

inline U64 LSB_v(U64 b)
{
	return b? (1ULL << LSB(b)) : 0;
}

inline U64 MSB_v(U64 b)
{
	return b? (1ULL << MSB(b)) : 0;
}