#pragma once
#include "types.h"
#include "RowCol_s.h"
#include "ChessMove.h"
#include "defs.h"
#include "../../HEADERS/Random/myRandom.h"
#include "BitWiseFunctions.h"
#include "Pos.h"
#include "countElsSent.h"
#include "../consoleChess2/perftFenx.h"
#include "../../HEADERS/max_min/max_min.h"
#include <initializer_list>
#include <cassert>
#include <string>
#include <thread>

class Board
{
public:
	U32 SELECTED_DEPTH = 4;
	// max number of moves in a position is 218, which is used as a literal
public:
	Board();
	//Board(const Board& other);
	U64& operator[](U32 index) { assert(index < bitBoards); return bitBoard[index]; }
	void initBitBoards();
	void initPawnMoves();
	void initKingMoves();
	void initKnightMoves();
	void initRookMoves();
	void initBishopMoves();
	void initQueenMoves();
	void initAllMoves();
	void initHashes();
	void printBoard();
	void printBitBoard(U64 b);
	void printPiece(int piece);
	void printCurrentMoves();

	void printFiles();
	void printRanks();

	void updateBitBoards();
	void updatePosHis();
	void parseFen(const char* fen);
	void addEnPasantSquare(U32 square);
public:
	void playGame();


public:
	//to do: void generateLegalMoves() to take into account being in check.
	void generatePawnMoves();
	void generateKnightMoves();
	void generateKingMoves();
	void generateRookMoves();
	void generateBishopMoves();
	void generateQueenMoves();
	
	void generateAllMoves();

	void removeRowBlockers(int index, U64& pieceMoves, int dirOffset);
	void removeColBlockers(int index, U64& pieceMoves, int dirOffset);
	void removeDiagBlockers(U32 index, U64& pieceMoves, U64 blockers);
	int getOpPiece(int piece);

public:
	U64 getRookAttacks();
	U64 getBishopAttacks();
	U64 getQueenAttacks();
	U64 getKnightAttacks();
	U64 getKingAttacks();
	U64 getPawnAttacks();
	U64 getPawnCaptureAttacks();

	U64 getAttacks(int piece);
	typedef U64(Board::*funcPtr)();
	
	 static constexpr funcPtr CaptureAttackFunctions[12]
	{
		&Board::getPawnCaptureAttacks,
		&Board::getKnightAttacks,
		&Board::getBishopAttacks,
		&Board::getRookAttacks,
		&Board::getQueenAttacks,
		&Board::getKingAttacks,

		&Board::getPawnCaptureAttacks,
		&Board::getKnightAttacks,
		&Board::getBishopAttacks,
		&Board::getRookAttacks,
		&Board::getQueenAttacks,
		&Board::getKingAttacks
	};

	bool isLegal(U32 move);
	bool isAttacked(U32 square, int attackerColor);
	Result getResult();
	MoveScore_s computerMove(U32 depth, int a = INT32_MIN, int b = INT32_MAX, bool* exit=nullptr); //, int a = INT32_MIN, int b = INT32_MAX);
	void compMove(Board b, U32 depth, MoveScore_s* ms, bool* isActive, bool* isDead, bool* finished);
	MoveScore_s computerMoveTh(U32 depth); //, int a = INT32_MIN, int b = INT32_MAX);
	MoveScore_s quietSearch(U32 depth, bool* exit = nullptr);
	MoveScore_s quietSearchSq(U32 depth, U32 move, bool* exit = nullptr);
	void doNothing() {};
	
	void perf(U32 depth);
	void perfPerMove(U32 depth);
	void perfPerMoveNoPrint(Pos& pos);
	void perfTestsTh(); // threaded perftests
	void perftFen(Pos* pos, bool* isThreadActive);
	void performanceTests();
	U32 totalMoves();
	void copyMoves(U32* avMoves);
	void swap(U32& move1, U32& move2);
	void swapIfCapture(U32& captureMove, U32& swapMove, U32& advanceIndex);
	U32 popcnt(U64 b);
	int eval();
	void initPosValues();
	int colValPawn(U32 col);
	static int POS_VALUE[12][64];
	int posValue(int piece, U64 b);
public:
	U64 perfMoves{ 0 };
	// perhaps I will later add invalid values so that we know where the list of totalMoves ends
	U32 P_list[128];
	U32 Prom_list[25];
	U32 N_list[128];
	U32 R_list[140];
	U32 B_list[140];
	U32 Q_list[280];
	U32 K_list[8];

	U32 totalPawnMoves{ 0 }; 
	U32 totalPromMoves{ 0 };
	U32 totalKnightMoves{ 0 };
	U32 totalRookMoves{ 0 };
	U32 totalBishopMoves{ 0 };
	U32 totalQueenMoves{ 0 };
	U32 totalKingMoves{ 0 };

	U32 totalAllMoves{ 0 };

	static U64 hashPieces[12][64];
	static U64 hashSideToMove;
	static U64 hashCastlingRights[4];
	static U64 hashEnPassant[8];
	U64 hashKey{ 0 };

public:
	static U64 wP_Moves[64];
	static U64 wP_Capture_Moves[64];
	static U64 bP_Moves[64];
	static U64 bP_Capture_Moves[64];

	static U64 K_Moves[64];

	static U64 N_Moves[64];
	static U64 R_Moves[64];
	static U64 B_Moves[64];
	static U64 Q_Moves[64];

	//ONLY THE MOVES THAT CAPTURE!
	static U64 All_Moves[12][64];
	
	U32 posHisSize{ 0 };

public:
	void playMove(U32 move);
	void undoMove(U32 move);
	void redoMove();
	

public:
	void printMoves(U32* list, U32 moves);
public:
	bool wturn{ true };
public:
	static constexpr U32 ORIGIN_SQ_MASK{ 63 };
	static constexpr U32 DEST_SQ_MASK{ ORIGIN_SQ_MASK << 6 };
	static constexpr U32 MOVE_TYPE_MASK{ DEST_SQ_MASK << 6 };
	static constexpr U32 PIECE_MASK{ MOVE_TYPE_MASK << 6 };
	static constexpr U32 PROM_MASK{ PIECE_MASK << 6 };
	static constexpr U32 FAKE_MOVE{ ~0UL };
	static constexpr int FAKE_SCORE{ 0 };
	static constexpr int WIN_SCORE{ 1'000'000'000 };
	static constexpr int DRAW_SCORE{ 0 };

	inline U32 getOrigin(U32 move);
	inline U32 getDest(U32 move);
	inline U32 getMoveType(U32 move);
	inline U32 getPiece(U32 move);
	inline U32 getPromPiece(U32 move);
public:
	static constexpr U32 bitBoards{ 12 }; // that's the number of bitBoards to represent the chess position
	static constexpr U32 MAX_MOVES{ 2048 };
	U64 posHis[MAX_MOVES][bitBoards]; //***
	U64 hashHis[MAX_MOVES];          //***
	U64 castlingHis[MAX_MOVES];      //***
public:

	U64 bitBoard[bitBoards]{ 0 };
	U64 allBitBoards{ 0 };
	U64 whiteBitBoards{ 0 };
	U64 blackBitBoards{ 0 };
	U64 emptyBitBoard{ 0 };
	U64 universe{ ~0ULL };
	U64 allZero{ 0LL };

	U64 castlingRights;

public:
	/*U64 wPromotions[64]{ 0 };
	U64 bPromotions[64]{ 0 };*/
public:
	U32 playerMove{ FAKE_MOVE };

public:
	void _printBitBoard(U64 b, const char* msg="");
	void _clearBoard_();
	void _go_through_the_moves();
	void _show_move(U32 move);
	void _add_piece(int piece, int sq);
	void _remove_piece(int piece, int sq);

	std::string getMove();
	U32 getPromotionPiece();
	inline bool isBtwn(int value, int l, int r);
	inline bool isValidLetter(char letter);
	inline bool isValidNumber(char number);
	inline U32 squareOf(U32 row, U32 col);
	inline RowCol_s getRowCol(U32 square);
	inline U32 getRow(U32 square);
	inline U32 getCol(U32 square);
	ChessMove convert(const std::string& str);
	bool isValidMove(const std::string& move);
	bool isPieceMove(ChessMove move, U32* list, U32 size); // sets playerMove
	bool isValidMove(ChessMove cm);
	inline U32 getCol(char letter) { return 'h' - letter; }
	inline U32 getRow(char number) { return number - '1'; }

	void clearPiece(U32 square);
	U64 getLastPieceOrgDst_bb(U32 piece);
public:
	U64 west(U32 square);
	U64 east(U32 square);
	U64 north(U32 square);
	U64 south(U32 square);

	static U64 west_b(U64 b);
	static U64 east_b(U64 b);
	static U64 north_b(U64 b);
	static U64 south_b(U64 b);

	static U64 northEast_b(U64 b);
	static U64 northWest_b(U64 b);
	static U64 southEast_b(U64 b);
	static U64 southWest_b(U64 b);

	U64 extendToEdge_b(U64 b, U64(*func)(U64) );
};





/* 
* IDEAS FOR IMPROVEMENT:
* add non legal moves when generating moves and then iterate over them to remove the non legal moves
* this way, isLegal(move) may be called less times
*/