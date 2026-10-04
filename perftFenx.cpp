#include "perftFenx.h"
#include "Board.h"


void perftFenx(Pos* pos, bool* isThreadActive)
{
	Board board;
	board.parseFen(pos->fen);
	for (int i{ 1 }; i <= pos->depth; ++i)
	{
		board.perf(i);
		pos->out[i - 1] = board.perfMoves;
		//std::cout << board.perfMoves << "\n\n\n";
		board.perfMoves = 0;
	}
	*isThreadActive = false;
}