#include "MoveScore_s.h"

bool operator<(const MoveScore_s& ms1, const MoveScore_s& ms2)
{
	return ms1.score < ms2.score;
}

bool operator>(const MoveScore_s& ms1, const MoveScore_s& ms2)
{
	return ms1.score > ms2.score;
}

bool operator==(const MoveScore_s& ms1, const MoveScore_s& ms2)
{
	return ms1.score == ms2.score;
}