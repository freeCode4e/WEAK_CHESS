#pragma once
using U32 = unsigned int;
struct MoveScore_s
{
	U32 move;
	int score;
};

bool operator<(const MoveScore_s& ms1, const MoveScore_s& ms2);
bool operator>(const MoveScore_s& ms1, const MoveScore_s& ms2);
bool operator==(const MoveScore_s& ms1, const MoveScore_s& ms2);