#include "Board.h"

#include "perftFenx.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>


U64 Board::wP_Moves[64]{};
U64 Board::bP_Moves[64]{};
U64 Board::wP_Capture_Moves[64]{};
U64 Board::bP_Capture_Moves[64]{};

U64 Board::N_Moves[64]{};
U64 Board::R_Moves[64]{};

U64 Board::K_Moves[64];
U64 Board::B_Moves[64];
U64 Board::Q_Moves[64];

U64 Board::All_Moves[12][64];
int Board::POS_VALUE[12][64];

U64 Board::hashPieces[12][64];
U64 Board::hashSideToMove;
U64 Board::hashCastlingRights[4];
U64 Board::hashEnPassant[8];

//U64 Board::posHis[MAX_MOVES][bitBoards];
//U64 Board::castlingHis[MAX_MOVES];
//
//U64 Board::hashHis[MAX_MOVES];

//Board::Board(const Board& other)
//{
//	for (int i{ wP }; i <= bK; ++i)
//	{
//		bitBoard[i] = other.bitBoard[i];
//	}
//	for (int i{ wP }; i < bP; ++i)
//	{
//		whiteBitBoards |= bitBoard[i];
//	}
//	for (int i{ bP }; i <= bK; ++i)
//	{
//		blackBitBoards |= bitBoard[i];
//	}
//	allBitBoards = (whiteBitBoards | blackBitBoards);
//	emptyBitBoard |= ~allBitBoards;
//	castlingRights = other.castlingRights;
//
//	for (U32 i{ 1 }; i <= other.posHisSize; ++i)
//	{
//		for (int piece{ wP }; piece <= bK; ++i)
//		{
//			posHis[MAX_MOVES - i][piece] = other.posHis[MAX_MOVES - i][piece];
//		}
//		castlingHis[MAX_MOVES - i] = other.castlingHis[MAX_MOVES - i];
//	}
//}

Board::Board()
{
	//std::cout << "constructor\n";
	initPosValues();
	initHashes();
	initBitBoards();
	initPawnMoves();
	initKnightMoves();
	initRookMoves();
	initBishopMoves();
	initQueenMoves();
	initKingMoves();
	initAllMoves();

}

void Board::parseFen(const char* fen)
{
	_clearBoard_();
	const char* pieces{ "PNBRQKpnbrqk" };
	const char* ptr{ pieces };
	int piecesIndex{ 0 };
	const char* current{ fen };
	int index{ 63 };

	//pieces
	while (current && *current != ' ' && index >= 0)
	{
		if (*current == '/')
		{
			++current;
			continue;
		}
		if (*current >= '1' && *current <= '8')
		{
			int i{ *current - '0' };
			index -= i;
			++current;
			continue;
		}
		ptr = pieces; piecesIndex = 0;
		while (ptr)
		{
			if (*current == *ptr)
			{
				bitBoard[piecesIndex] |= (1ULL << index);
				break;
			}
			++ptr; ++piecesIndex;
		}
		++current; --index;
	}
	++current;
	//side to move
	wturn = (*current == 'w');
	current += 2;

	//castling rights
	while (current && *current != ' ')
	{
		if (*current == 'K')
			castlingRights |= (1ULL << 1);
		else if (*current == 'Q')
			castlingRights |= (1ULL << 5);
		else if (*current == 'k')
			castlingRights |= (1ULL << 57);
		else if (*current == 'q')
			castlingRights |= (1ULL << 61);
		++current;
	}
	++current;
	//enpasant square
	
	if (*current != '-')
	{
		char first{ *current - ('a' - 'A') }; // make it capital
		first = 'H' - (first - 'A');
		++current;
		char second{ *current };
		U32 col{ (U32)(first - 'A') };
		U32 row{ (U32) (second - '1') };
		U32 sq{ squareOf(row, col) };
		addEnPasantSquare(sq);
	}
	updateBitBoards();
	updatePosHis();
}

void Board::addEnPasantSquare(U32 square)
{
	if (wturn)
	{
		clear_bit(bitBoard[bP], square - 8);
		set_bit(bitBoard[bP], square + 8);
		updateBitBoards();
		updatePosHis();
		clear_bit(bitBoard[bP], square + 8);
		set_bit(bitBoard[bP], square - 8);
	}
	else
	{
		clear_bit(bitBoard[wP], square + 8);
		set_bit(bitBoard[wP], square - 8);
		updateBitBoards();
		updatePosHis();
		clear_bit(bitBoard[wP], square - 8);
		set_bit(bitBoard[wP], square + 8);
	}
}
 
void Board::initBitBoards()
{
	bitBoard[wP] = M_RANK::RANK_2;
	bitBoard[bP] = M_RANK::RANK_7;

	bitBoard[wR] = (1ULL) | (1ULL << 7);
	bitBoard[wN] = (1ULL << 1 ) | (1ULL << 6);
	bitBoard[wB] = (1ULL << 2)  | (1ULL << 5);
	bitBoard[wK] = (1ULL << 3);
	bitBoard[wQ] = (1ULL << 4);

	// black pieces opposite to white pieces, shifting 56 places does the trick.
	bitBoard[bR] = bitBoard[wR] << 56; 
	bitBoard[bN] = bitBoard[wN] << 56;
	bitBoard[bB] = bitBoard[wB] << 56;
	bitBoard[bK] = bitBoard[wK] << 56;
	bitBoard[bQ] = bitBoard[wQ] << 56;


	for (int i{ wP }; i <= bK; ++i)
	{
		allBitBoards |= bitBoard[i];
		//posHis[MAX_SIZE - posHisSize][i] = bitBoard[i];
	}
	for (int i{ wP }; i < bP; ++i)
	{
		whiteBitBoards |= bitBoard[i];
	}
	for (int i{ bP }; i <= bK; ++i)
	{
		blackBitBoards |= bitBoard[i];
	}

	emptyBitBoard |= ~allBitBoards;
	castlingRights = ((bitBoard[wK] | bitBoard[bK]) << 2) | ((bitBoard[wK] | bitBoard[bK]) >> 2);
	updatePosHis();
	//_printBitBoard(castlingRights);
	//std::cout << allBitBoards;
	//bitBoard[bN] = (1ULL << 57) | (1ULL << 62);
}

void Board::updateBitBoards()
{
	allBitBoards = 0;
	whiteBitBoards = 0;
	blackBitBoards = 0;
	for (int i{ wP }; i <= bK; ++i)
	{
		allBitBoards |= bitBoard[i];
		//posHis[MAX_SIZE - posHisSize][i] = bitBoard[i];
	}
	for (int i{ wP }; i < bP; ++i)
	{
		whiteBitBoards |= bitBoard[i];
	}
	for (int i{ bP }; i <= bK; ++i)
	{
		blackBitBoards |= bitBoard[i];
	}

	emptyBitBoard = ~allBitBoards;

	/*_printBitBoard(emptyBitBoard, "empty");
	_printBitBoard(allBitBoards, "all");
	_printBitBoard(whiteBitBoards, "white");
	_printBitBoard(blackBitBoards, "black");*/
}

void Board::printBoard()
{
	bool isEmpty;
	for (U32 sq{ 0 }; sq < 64; ++sq) {
		isEmpty = true;
		for (int piece{ wP }; piece <= bK; ++piece) {
			if (get_bit(bitBoard[piece], sq)) {
				printPiece(piece);
				isEmpty = false;
				break;
			}
		}
		if (isEmpty)
			std::cout << std::setw(4) << "--";
		if ((sq + 1) % 8 == 0)
			std::cout << "\n\n";
	}
}

void Board::printBitBoard(U64 b)
{
	for (U32 sq{ 0 }; sq < 64; ++sq) {
		std::cout << (bool)get_bit(b, sq) << "   ";
		if ((sq + 1) % 8 == 0)
			std::cout << "\n\n";
	}
}

void Board::printPiece(int piece)
{
	//wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK
	if (piece == wP)
		std::cout<<std::setw(4) << "wP";
	else if (piece == wN)
		std::cout<<std::setw(4) << "wN";
	else if (piece == wB)
		std::cout<<std::setw(4) << "wB";
	else if (piece == wR)
		std::cout<<std::setw(4) << "wR";
	else if (piece == wQ)
		std::cout<<std::setw(4) << "wQ";
	else if (piece == wK)
		std::cout<<std::setw(4) << "wK";
	else if (piece == bP)
		std::cout<<std::setw(4) << "bP";
	else if (piece == bN)
		std::cout<<std::setw(4) << "bN";
	else if (piece == bB)
		std::cout<<std::setw(4) << "bB";
	else if (piece == bR)
		std::cout<<std::setw(4) << "bR";
	else if (piece == bQ)
		std::cout<<std::setw(4) << "bQ";
	else
		std::cout << std::setw(4) << "bK";
}

void Board::initPawnMoves()
{
	U64 pawns = 0ULL;
	for (int i{ 0 }; i < 64; ++i)
	{
		set_bit(pawns, i);
		wP_Moves[i] = (pawns << 8) | ((pawns & M_RANK::RANK_2) << 16);
		wP_Capture_Moves[i] = (pawns << 7 & ~M_FILE::FILE_A) | (pawns << 9 & ~M_FILE::FILE_H);

		//wP_Moves[i] &= (~allBitBoards ; 
		//wP_Capture_Moves[i] &= blackBitBoards;
		clear_bit(pawns, i);
	}

	for (int i{ 0 }; i < 64; ++i)
	{
		set_bit(pawns, i);
		bP_Moves[i] = (pawns >> 8) | ((pawns & M_RANK::RANK_7) >> 16);
		bP_Capture_Moves[i] = (pawns >> 7 & ~M_FILE::FILE_H) | (pawns >> 9 & ~M_FILE::FILE_A);

		//bP_Moves[i] &= ~allBitBoards;
		//bP_Capture_Moves[i] &= whiteBitBoards;
		clear_bit(pawns, i);
	}
}

void Board::initKingMoves()
{
	U64 kingMoves{ 0ULL };
	for (int i{ 0 }; i < 64; ++i)
	{
		set_bit(kingMoves, i);

		K_Moves[i] =
			west_b(kingMoves) | east_b(kingMoves) |
			south_b(kingMoves) | north_b(kingMoves) |
			northEast_b(kingMoves) | northWest_b(kingMoves) |
			southEast_b(kingMoves) | southWest_b(kingMoves);

		clear_bit(kingMoves, i);
	}
}

void Board::initBishopMoves()
{
	U64 bishopMoves{ 0ULL };

	for (int i{ 0 }; i < 64; ++i)
	{
		set_bit(bishopMoves, i);
		U64 bm{ bishopMoves };
		bishopMoves |= extendToEdge_b(bm, northEast_b);
		bishopMoves |= extendToEdge_b(bm, northWest_b);
		bishopMoves |= extendToEdge_b(bm, southEast_b);
		bishopMoves |= extendToEdge_b(bm, southWest_b);
		B_Moves[i] = bishopMoves;
		bishopMoves = 0ULL;
	}
}

void Board::initQueenMoves()
{
	for (int i{ 0 }; i < 64; ++i)
	{
		Q_Moves[i] = R_Moves[i] | B_Moves[i];
	}
}

void Board::initKnightMoves()
{
	U64 knights{ 0ULL };
	for (int i{ 0 }; i < 64; ++i)
	{
		set_bit(knights, i);
		N_Moves[i] =
			(knights << 15 & ~M_FILE::FILE_A) | (knights << 17 & ~M_FILE::FILE_H)  |
			(knights >> 15 & ~M_FILE::FILE_H) | (knights >> 17 & ~M_FILE::FILE_A)  |
			(knights << 6 & ~M_FILE::FILE_AB) | (knights << 10 & ~M_FILE::FILE_GH) |
			(knights >> 6 & ~M_FILE::FILE_GH) | (knights >> 10 & ~M_FILE::FILE_AB);
		clear_bit(knights, i);
	}
}

void Board::initRookMoves()
{
	U64 rooks{ 0ULL };
	U64 rank{ M_RANK::RANK_1 };
	U64 file{ M_FILE::FILE_H };
	for (int i{ 0 }; i < 64; ++i)
	{
		set_bit(rooks, i);
		R_Moves[i] = 
			((rooks ^ rank) /*& (~M_FILE::FILE_A & ~M_FILE::FILE_H))*/ | ((rooks ^ file) /*& ~M_RANK::RANK_1 & ~M_RANK::RANK_8*/));
		clear_bit(rooks, i);

		if ((i + 1) % 8 == 0) {
			rank <<= 8;
			file >>= 7;
		}
		else
			file <<= 1;
	}
}

void Board::initAllMoves()
{
	// ONLY THE MOVES THAT COULD CAPTURE!
	for (int i{ 0 }; i < 64; ++i)
	{
		All_Moves[wP][i] = (wP_Capture_Moves[i]);
		All_Moves[bP][i] = (bP_Capture_Moves[i]);

		All_Moves[wN][i] = (N_Moves[i]);
		All_Moves[bN][i] = (N_Moves[i]);

		All_Moves[wB][i] = (B_Moves[i]);
		All_Moves[bB][i] = (B_Moves[i]);

		All_Moves[wR][i] = (R_Moves[i]);
		All_Moves[bR][i] = (R_Moves[i]);

		All_Moves[wQ][i] = (Q_Moves[i]);
		All_Moves[bQ][i] = (Q_Moves[i]);

		All_Moves[wK][i] = (K_Moves[i]);
		All_Moves[bK][i] = (K_Moves[i]);
	}
}

void Board::initHashes()
{
	for (U32 sq{ 0 }; sq < 64; ++sq)
	{
		for (U32 i{ wP }; i <= bK; ++i)
		{
			hashPieces[i][sq] = Random::get(0ULL, ~0ULL);
		}
	}
	for (U32 i{ 0 }; i < 4; ++i)
	{
		hashCastlingRights[i] = Random::get(0ULL, ~0ULL);
	}
	for (U32 i{ FILE_A }; i <= FILE_H; ++i)
	{
		hashEnPassant[i] = Random::get(0ULL, ~0ULL);
	}
	hashSideToMove = Random::get(0ULL, ~0ULL);
}

void Board::printFiles()
{
	for (int i{ FILE_A }; i <= FILE_H; ++i)
	{
		printBitBoard(M_FILE::FILE_A >> i);
		std::cin.get();
		system("cls");
	}
}

void Board::printRanks()
{
	for (int i{ RANK_1 }; i <= RANK_8; ++i) {
		printBitBoard(M_RANK::RANK_1 << i * 8);
		std::cin.get();
		system("cls");
	}
}

U64 Board::west(U32 square)
{
	return ((square != 0) && (square < 64)) ? (get_bit(universe, square - 1) & ~M_FILE::FILE_A) : (0ULL);
}
U64 Board::east(U32 square) { return (square < 63) ? (get_bit(universe, square+1) & ~M_FILE::FILE_H) : (0ULL); }
U64 Board::north(U32 square) { return (square < 56) ? (get_bit(universe, square + 8)) : (0ULL); }
U64 Board::south(U32 square) { return (square > 7 && square < 64) ? (get_bit(universe, square - 8)) : (0ULL); }

U64 Board::west_b(U64 b) { return (b << 1) & (~M_FILE::FILE_H); }
U64 Board::east_b(U64 b) { return (b >> 1) & (~M_FILE::FILE_A); }
U64 Board::south_b(U64 b) { return (b >> 8); }
U64 Board::north_b(U64 b) { return (b << 8); }

U64 Board::northEast_b(U64 b) 
{	
	b = north_b(b); 
	b = east_b(b); 
	return b; 
}
U64 Board::northWest_b(U64 b) 
{ 
	b = north_b(b); 
	b = west_b(b); 
	return b; 
}
U64 Board::southEast_b(U64 b) 
{ 
	b = south_b(b); 
	b = east_b(b); 
	return b; 
}
U64 Board::southWest_b(U64 b) 
{ 
	b = south_b(b); 
	b = west_b(b);
	return b; 
}

U64 Board::extendToEdge_b(U64 b, U64(*func)(U64))
{
	U64 res{ b };
	//_printBitBoard(b);
	while (res) {
		res = func(res);
		b |= res;
		//_printBitBoard(b);
	}
	return b;
}

U64 Board::getLastPieceOrgDst_bb(U32 piece)
{
	// origin and destination of last piece moved bitboard; get those with LSB_v and MSB_v potentially.
	return bitBoard[piece] ^ posHis[MAX_MOVES - (posHisSize - 1)][piece];
}

void Board::generatePawnMoves()
{
	// first 6 bits are origin square
	// the next 6 bits are destination square
	// the next 6 bits are for a flag so we know the type of move eg: a capture, or en passant etc.
	// the next 6 bits are for which piece moved, xg white pawn, or black pawn.
	// the last 6 bits are for promotion piece.
	totalPawnMoves = 0;
	totalPromMoves = 0;
	U32 move;
	if (wturn)
	{
		U64 pos = bitBoard[wP];
		while (pos)
		{
			U32 lsb{ pop_LSB(pos) };
			U64 pawnMoves{ (wP_Moves[lsb] & emptyBitBoard) };
			U64 blockers{ M_RANK::RANK_3 & allBitBoards };
			pawnMoves &= (~(blockers << 8) | (1ULL << (lsb + 8)));

			U64 rank{ M_RANK::RANK_1 << 8 * getRow(lsb) };
			U64 enPassantRank{ rank & M_RANK::RANK_5 };
			U64 lastPawnMoveStart{ MSB_v(getLastPieceOrgDst_bb(bP)) };
			U64 lastPawnMoveEnd{ LSB_v(getLastPieceOrgDst_bb(bP)) & (lastPawnMoveStart >> 16) };
			U64 enPasants{ enPassantRank & lastPawnMoveEnd & (east(lsb) | west(lsb)) };
			pawnMoves |= (enPasants << 8);
			pawnMoves |= (wP_Capture_Moves[lsb] & blackBitBoards);
			


			while (pawnMoves)
			{
				U32 dest{ pop_LSB(pawnMoves) };
				U64 enPasDest{ (enPasants << 8) & (1ULL << dest) };
				U32 type{ 0 };
				if ((1ULL << dest) & blackBitBoards)
				{
					type |= (1ULL << capture);
				}
				if (getRow(dest) == 7)
				{
					type |= (1ULL << promotion);
				}
				if (enPasDest)
				{
					type |= (1ULL << enpasant);
				}
				if (!type)
					set_bit(type, quiet);

				U32 pieceToMove{ wP };
				{
					if (get_bit(type, promotion)) {
						move = lsb | (dest << 6) | (type << 12) | (pieceToMove << 18) | ((static_cast<U32>(wN)) << 24);
						if (isLegal(move)) {
							Prom_list[totalPromMoves++] = move;
							for (U32 i{ wB }; i <= wQ; ++i) {
								Prom_list[totalPromMoves++] = ((move & (~PROM_MASK)) | (i << 24));
							}
						}
					}
					else {
						move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
						if (isLegal(move))
							P_list[totalPawnMoves++] = move;
					}
						
				}
			}
		}
	}
	else
	{
		U64 pos = bitBoard[bP];
		while (pos)
		{
			U32 lsb{ pop_LSB(pos) };
			U64 pawnMoves{ (bP_Moves[lsb] & emptyBitBoard) };
			U64 blockers{ M_RANK::RANK_6 & allBitBoards };
			pawnMoves &= (~(blockers >> 8) | (1ULL << (lsb - 8)));

			U64 rank{ M_RANK::RANK_1 << 8 * getRow(lsb) };
			U64 enPassantRank{ rank & M_RANK::RANK_4 };
			U64 lastPawnMoveStart{ LSB_v(getLastPieceOrgDst_bb(wP)) };
			U64 lastPawnMoveEnd{ MSB_v(getLastPieceOrgDst_bb(wP)) & (lastPawnMoveStart << 16) };
			U64 enPasants{ enPassantRank & lastPawnMoveEnd & (east(lsb) | west(lsb)) };
			pawnMoves |= (enPasants >> 8);
			pawnMoves |= (bP_Capture_Moves[lsb] & whiteBitBoards);


			while (pawnMoves)
			{
				U32 dest{ pop_LSB(pawnMoves) };
				U64 enPasDest{ (enPasants >> 8) & (1ULL << dest) };

				U32 type{ 0 };
				if ((1ULL << dest) & whiteBitBoards)
				{
					type |= (1ULL << capture);
				}
				if (getRow(dest) == 0)
				{
					type |= (1ULL << promotion);
				}
				if (enPasDest)
				{
					type |= (1ULL << enpasant);
				}
				if (!type)
					set_bit(type, quiet);
				U32 pieceToMove{ bP };
				 {
					if (get_bit(type, promotion)) {
						move = lsb | (dest << 6) | (type << 12) | (pieceToMove << 18) | ((static_cast<U32>(bN)) << 24);
						if (isLegal(move))
						{
							Prom_list[totalPromMoves++] = move;
							for (U32 i{ bB }; i <= bQ; ++i) {
								Prom_list[totalPromMoves++] = ((move & (~PROM_MASK)) | (i << 24));
							}
						}
					}
					else {
						move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
						if (isLegal(move))
							P_list[totalPawnMoves++] = move;
					}
						
					
				}
			}
		}
	}
}

void Board::generateKingMoves()
{
	totalKingMoves = 0;
	U64 pos;
	U32 lsb;
	U32 dest;
	U32 type;
	U32 pieceToMove;
	U64 kingMoves;
	
	U64 notOwnBitBoard;
	U64 opBitBoard;

	U64 rank;
	U64 file{ M_FILE::FILE_E };

	U32 move;

	if (wturn)
	{
		pieceToMove = wK;
		notOwnBitBoard = ~whiteBitBoards;
		opBitBoard = blackBitBoards;
		rank = M_RANK::RANK_1;
	}
	else
	{
		pieceToMove = bK;
		notOwnBitBoard = ~blackBitBoards;
		opBitBoard = whiteBitBoards;
		rank = M_RANK::RANK_8;

	}
	pos = bitBoard[pieceToMove];
	while (pos)
	{
		lsb = pop_LSB(pos);
		kingMoves = K_Moves[lsb] & notOwnBitBoard;
		//_printBitBoard(kingMoves, "kingMoves");
		while (kingMoves)
		{
			dest = pop_LSB(kingMoves);
			type = ((1ULL << dest) & opBitBoard) ? (1ULL << capture) : (1ULL << quiet);
			move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
			if(isLegal(move))
				K_list[totalKingMoves++] = move;
		}
	}
	U64 castlingMoves = bitBoard[pieceToMove] & rank & file;
#if _DEBUG
	if (!bitBoard[pieceToMove]) {
		std::cout << "pieceToMove: " << pieceToMove << '\n';
		_printBitBoard(bitBoard[pieceToMove], "pieceToMove bitboard");
		std::cout << "\n\n\n";
		printBoard();
	}
#endif
	U32 kingSq{ LSB(bitBoard[pieceToMove])};
	bool kingIsAttacked{ isAttacked(kingSq, (int)!wturn) };
	bool leftIsAttacked{ isAttacked(kingSq - 1, (int)!wturn)};
	bool rightIsAttacked{ isAttacked(kingSq + 1, (int)!wturn)};

	U64 kingNotAttacked{ static_cast<U64>((!kingIsAttacked)) };
	U64 leftNotAttacked{ static_cast<U64>((!leftIsAttacked)) };
	U64 rightNotAttacked{ static_cast<U64>((!rightIsAttacked)) };

	castlingMoves =
		(((castlingMoves << 2) & emptyBitBoard & (emptyBitBoard << 1) & (emptyBitBoard >> 1)) &
			(kingNotAttacked << (kingSq + 2)) &
			(rightNotAttacked << (kingSq + 2))) |

		(((castlingMoves >> 2) & emptyBitBoard & (emptyBitBoard >> 1)) &
			(kingNotAttacked << (kingSq - 2)) &
			(leftNotAttacked << (kingSq - 2)));
	//castlingMoves |= (castlingRights & rank);
	//_printBitBoard(castlingMoves, "castlingMoves before:");
	//_printBitBoard(castlingRights);
	castlingMoves &= castlingRights;
	//_printBitBoard(castlingMoves, "castlingMoves after:");
	while (castlingMoves)
	{
		dest = pop_LSB(castlingMoves);
		type = (1ULL << castling);
		move = (kingSq | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
		if (isLegal(move))
			K_list[totalKingMoves++] = move;
	}
}

void Board::generateKnightMoves()
{
	totalKnightMoves = 0;
	U64 pos;
	U32 lsb;
	U32 dest;
	U32 type;      
	U32 pieceToMove;
	U64 knightMoves;

	U32 move;
	if (wturn)
	{
		pos = bitBoard[wN];
		while (pos)
		{
			lsb = pop_LSB(pos);
			knightMoves = N_Moves[lsb] & ~whiteBitBoards;
			/*_printBitBoard(whiteBitBoards, "white");
			_printBitBoard(knightMoves, "knight");*/
			while (knightMoves)
			{
				dest = pop_LSB(knightMoves);
				type = ((1ULL << dest) & blackBitBoards) ? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = wN;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~(PROM_MASK));
				if (isLegal(move))
					N_list[totalKnightMoves++] = move;
			}
		}
	}
	else
	{
		pos = bitBoard[bN];
		while (pos)
		{
			lsb = pop_LSB(pos);
			knightMoves = (N_Moves[lsb] & ~blackBitBoards);
			while (knightMoves)
			{
				dest = pop_LSB(knightMoves);
				type = ((1ULL << dest) & whiteBitBoards) ? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = bN;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~(PROM_MASK));
				if (isLegal(move))
					N_list[totalKnightMoves++] = move;
			}
		}
	}
}


void Board::generateAllMoves()
{
	generatePawnMoves();
	generateKnightMoves();
	generateRookMoves();
	generateBishopMoves();
	generateQueenMoves();
	generateKingMoves();
}

void Board::removeRowBlockers(int index, U64& pieceMoves, int dirOffset)
{
	int row{ index / 8 };
	index += dirOffset;
	int loopRow{ index / 8 };

	while (loopRow == row && index >= 0) 
	{
		if (get_bit(pieceMoves, index) & allBitBoards) {
			index += dirOffset;
			loopRow = index / 8;
			while (loopRow == row && index >= 0) {
				clear_bit(pieceMoves, index);
				index += dirOffset;
				loopRow = index / 8;
			}
			return;
		}
		index += dirOffset;
		loopRow = index / 8;
	}
}
void Board::removeColBlockers(int index, U64& pieceMoves, int dirOffset)
{
	
	index += dirOffset;
	
	while (index < 64 && index > -1)
	{
		if (get_bit(pieceMoves, index) & allBitBoards) {
			index += dirOffset;
			while (index < 64 && index > -1) {
				clear_bit(pieceMoves, index);
				index += dirOffset;
			}
			return;
		}
		index += dirOffset;
	}
}

void Board::generateRookMoves()
{
	totalRookMoves = 0;
	U64 pos;
	U64 rookMoves;
	U32 dest;
	U32 type;
	U32 lsb;
	U32 pieceToMove;

	U32 move;

	if (wturn)
	{
		pos = bitBoard[wR];
		while (pos)
		{
			lsb = pop_LSB(pos);
			rookMoves = (R_Moves[lsb]);
			removeRowBlockers(lsb, rookMoves, 1);
			removeRowBlockers(lsb, rookMoves, -1);
			removeColBlockers(lsb, rookMoves, 8);
			removeColBlockers(lsb, rookMoves, -8);
			rookMoves &= ~whiteBitBoards;
			
			while (rookMoves)
			{
				dest = pop_LSB(rookMoves);
				type = ((1ULL << dest) & blackBitBoards) ? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = wR;
				move = lsb | (dest << 6) | (type << 12) | (pieceToMove << 18);
				if(isLegal(move))
					R_list[totalRookMoves++] = move;
			}
		}
	}
	else
	{
		pos = bitBoard[bR];
		while (pos)
		{
			lsb = pop_LSB(pos);
			rookMoves = (R_Moves[lsb]);
			removeRowBlockers(lsb, rookMoves, 1);
			removeRowBlockers(lsb, rookMoves, -1);
			removeColBlockers(lsb, rookMoves, 8);
			removeColBlockers(lsb, rookMoves, -8);
			rookMoves &= ~blackBitBoards;

			while (rookMoves)
			{
				dest = pop_LSB(rookMoves);
				type = ((1ULL << dest) & whiteBitBoards) ? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = bR;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
				if (isLegal(move))
					R_list[totalRookMoves++] = move;
			}
		}
	}

}

void Board::removeDiagBlockers(U32 index, U64& pieceMoves, U64 blockers)
{
	U32 lsb;
	U32 row;
	U32 Brow;
	U32 col;
	U32 Bcol;
	U64 toEdge;
	/*_printBitBoard(pieceMoves, "piecemoves");
	_printBitBoard(blockers, "blockers");*/
	while (blockers)
	{
		lsb = LSB(blockers);
		row = getRow(lsb);
		Brow = getRow(index);
		Bcol = getCol(index);
		col = getCol(lsb);
		if (row < Brow && col < Bcol)
		{
			toEdge = extendToEdge_b(get_bit(blockers, lsb), southEast_b);
			clear_bit(toEdge, lsb);
			pieceMoves &= (~toEdge);
		}
		else if (row < Brow && col > Bcol)
		{
			toEdge = extendToEdge_b(get_bit(blockers, lsb), southWest_b);
			clear_bit(toEdge, lsb);
			pieceMoves &= (~toEdge);
		}
		else if (row > Brow && col < Bcol)
		{
			toEdge = extendToEdge_b(get_bit(blockers, lsb), northEast_b);
			//_printBitBoard(toEdge, "toEdge");
			clear_bit(toEdge, lsb);
			//_printBitBoard(toEdge, "toEdge");
			pieceMoves &= (~toEdge);
			//_printBitBoard(pieceMoves, "pieceMoves");
		}
		else
		{
			toEdge = extendToEdge_b(get_bit(blockers, lsb), northWest_b);
			//_printBitBoard(toEdge, "toEdge");
			clear_bit(toEdge, lsb);
			//_printBitBoard(toEdge, "toEdge");
			pieceMoves &= (~toEdge);
			//_printBitBoard(pieceMoves, "pieceMoves");
		}

		clear_bit(blockers, lsb);
	}
	//_printBitBoard(pieceMoves, "final");
}


void Board::generateBishopMoves()
{
	totalBishopMoves = 0;
	U64 pos;
	U64 bishopMoves;
	U32 dest;
	U32 type;
	U32 lsb;
	U32 pieceToMove;
	U32 move;
	if (wturn)
	{
		pos = bitBoard[wB];
		while (pos)
		{
			lsb = pop_LSB(pos);
			U64 blockers = B_Moves[lsb] & allBitBoards;
			clear_bit(blockers, lsb);
			bishopMoves = B_Moves[lsb];

			removeDiagBlockers(lsb, bishopMoves, blockers);
			bishopMoves &= ~whiteBitBoards;
			//_printBitBoard(bishopMoves);
			while (bishopMoves)
			{
				dest = pop_LSB(bishopMoves);
				type = ((1ULL << dest) & blackBitBoards)? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = wB;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
				if (isLegal(move))
					B_list[totalBishopMoves++] = move;
			}

		}
	}
	else
	{
		pos = bitBoard[bB];
		while (pos)
		{
			lsb = pop_LSB(pos);
			U64 blockers = B_Moves[lsb] & allBitBoards;
			clear_bit(blockers, lsb);
			bishopMoves = B_Moves[lsb];

			removeDiagBlockers(lsb, bishopMoves, blockers);
			bishopMoves &= ~blackBitBoards;
			while (bishopMoves)
			{
				dest = pop_LSB(bishopMoves);
				type = ((1ULL << dest) & whiteBitBoards) ? 1ULL << capture : 1ULL << quiet;
				pieceToMove = bB;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
				if (isLegal(move))
					B_list[totalBishopMoves++] = move;
			}
		}
	}
}

void Board::generateQueenMoves()
{
	
	totalQueenMoves = 0;
	U64 pos;
	U64 queenMovesB;
	U64 queenMovesR;
	U64 queenMoves;
	U32 dest;
	U32 type;
	U32 lsb;
	U32 pieceToMove;

	U32 move;
	if (wturn)
	{
		pos = bitBoard[wQ];
		while (pos)
		{
			lsb = pop_LSB(pos);
			U64 blockers = B_Moves[lsb] & allBitBoards;
			clear_bit(blockers, lsb);
			queenMovesB = B_Moves[lsb];

			removeDiagBlockers(lsb, queenMovesB, blockers);
			queenMovesB &= ~whiteBitBoards;

			//--------------------------------------

			queenMovesR = (R_Moves[lsb]);
			removeRowBlockers(lsb, queenMovesR, 1);
			removeRowBlockers(lsb, queenMovesR, -1);
			removeColBlockers(lsb, queenMovesR, 8);
			removeColBlockers(lsb, queenMovesR, -8);
			queenMovesR &= ~whiteBitBoards;

			queenMoves = (queenMovesB | queenMovesR);
			while (queenMoves)
			{
				dest = pop_LSB(queenMoves);
				type = ((1ULL << dest) & blackBitBoards) ? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = wQ;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
				if (isLegal(move))
					Q_list[totalQueenMoves++] = move;
			}

		}
	}
	else
	{
		pos = bitBoard[bQ];
		while (pos)
		{
			lsb = pop_LSB(pos);
			U64 blockers = B_Moves[lsb] & allBitBoards;
			clear_bit(blockers, lsb);
			queenMovesB = B_Moves[lsb];

			removeDiagBlockers(lsb, queenMovesB, blockers);
			queenMovesB &= ~blackBitBoards;

			//--------------------------------------

			queenMovesR = (R_Moves[lsb]);
			removeRowBlockers(lsb, queenMovesR, 1);
			removeRowBlockers(lsb, queenMovesR, -1);
			removeColBlockers(lsb, queenMovesR, 8);
			removeColBlockers(lsb, queenMovesR, -8);
			queenMovesR &= ~blackBitBoards;

			queenMoves = (queenMovesB | queenMovesR);
			while (queenMoves)
			{
				dest = pop_LSB(queenMoves);
				type = ((1ULL << dest) & whiteBitBoards) ? (1ULL << capture) : (1ULL << quiet);
				pieceToMove = bQ;
				move = (lsb | (dest << 6) | (type << 12) | (pieceToMove << 18)) & (~PROM_MASK);
				if (isLegal(move))
					Q_list[totalQueenMoves++] = move;
			}

		}
	}

}

U64 Board::getRookAttacks()
{
	U32 lsb;
	U64 rookMoves;
	U64 pos;
	U64 notOwnBitBoards;

	U64 finalRookMoves{ 0ULL };
	/*system("cls");
	printBoard();*/
	if (wturn)
	{
		pos = bitBoard[wR];
		notOwnBitBoards = ~whiteBitBoards;
	}
	else
	{
		pos = bitBoard[bR];
		notOwnBitBoards = ~blackBitBoards;
	}

	while (pos)
	{
		lsb = pop_LSB(pos);
		rookMoves = (R_Moves[lsb]);
		removeRowBlockers(lsb, rookMoves, 1);
		removeRowBlockers(lsb, rookMoves, -1);
		removeColBlockers(lsb, rookMoves, 8);
		removeColBlockers(lsb, rookMoves, -8);
		rookMoves &= notOwnBitBoards;
		finalRookMoves |= rookMoves;
	}
	return finalRookMoves;
}

U64 Board::getBishopAttacks()
{
	U32 lsb;
	U64 pos;
	U64 bishopMoves;
	U64 notOwnBitBoards;

	U64 finalBishopMoves{ 0ULL };

	if (wturn)
	{
		pos = bitBoard[wB];
		notOwnBitBoards = ~whiteBitBoards;
	}
	else
	{
		pos = bitBoard[bB];
		notOwnBitBoards = ~blackBitBoards;
	}

	while (pos)
	{
		lsb = pop_LSB(pos);
		U64 blockers = B_Moves[lsb] & allBitBoards;
		clear_bit(blockers, lsb);
		bishopMoves = B_Moves[lsb];

		removeDiagBlockers(lsb, bishopMoves, blockers);
		bishopMoves &= notOwnBitBoards;
		finalBishopMoves |= bishopMoves;
	}
	return finalBishopMoves;
}

U64 Board::getQueenAttacks()
{
	U32 lsb;
	U64 pos;
	U64 finalQueenMoves{ 0ULL };
	U64 queenMovesB;
	U64 queenMovesR;
	U64 notOwnBitBoards;

	if (wturn)
	{
		pos = bitBoard[wQ];
		notOwnBitBoards = ~whiteBitBoards;
	}
	else
	{
		pos = bitBoard[bQ];
		notOwnBitBoards = ~blackBitBoards;
	}
	while (pos)
	{
		lsb = pop_LSB(pos);
		U64 blockers = B_Moves[lsb] & allBitBoards;
		clear_bit(blockers, lsb);
		queenMovesB = B_Moves[lsb];

		removeDiagBlockers(lsb, queenMovesB, blockers);
		queenMovesB &= notOwnBitBoards;

		//--------------------------------------

		queenMovesR = (R_Moves[lsb]);
		removeRowBlockers(lsb, queenMovesR, 1);
		removeRowBlockers(lsb, queenMovesR, -1);
		removeColBlockers(lsb, queenMovesR, 8);
		removeColBlockers(lsb, queenMovesR, -8);
		queenMovesR &= notOwnBitBoards;

		finalQueenMoves |= (queenMovesB | queenMovesR);
	}
	return finalQueenMoves;
}

U64 Board::getKnightAttacks()
{
	U32 lsb;
	U64 knightMoves;
	U64 finalKnightMoves{ 0ULL };
	U64 pos;
	U64 notOwnBitBoard;

	if (wturn)
	{
		pos = bitBoard[wN];
		notOwnBitBoard = ~whiteBitBoards;
	}
	else
	{
		pos = bitBoard[bN];
		notOwnBitBoard = ~blackBitBoards;
	}
	while (pos)
	{
		lsb = pop_LSB(pos);
		knightMoves = N_Moves[lsb] & notOwnBitBoard;
		finalKnightMoves |= knightMoves;
	}
	return finalKnightMoves;
}

U64 Board::getKingAttacks()
{
	U32 lsb;
	U64 kingMoves;
	U64 finalKingMoves{ 0ULL };
	U64 pos;
	U64 notOwnBitBoard;

	if (wturn)
	{
		pos = bitBoard[wK];
		notOwnBitBoard = ~whiteBitBoards;
	}
	else
	{
		pos = bitBoard[bK];
		notOwnBitBoard = ~blackBitBoards;
	}
	while (pos)
	{
		lsb = pop_LSB(pos);
		kingMoves = K_Moves[lsb] & notOwnBitBoard;
		finalKingMoves |= kingMoves;
	}
	return finalKingMoves;
}

U64 Board::getPawnAttacks()
{
	U64 pos;
	U32 lsb;
	U64 pawnMoves;
	U64 finalPawnMoves{ 0ULL };
	U64 blockers;
	U64 rank;
	U64 enPassantRank;
	U64 lastPawnMoveStart;
	U64 lastPawnMoveEnd;
	U64 enPasants;

	if (wturn)
	{
		pos = bitBoard[wP];
		while (pos)
		{
			lsb = pop_LSB(pos);
			pawnMoves = (wP_Moves[lsb] & emptyBitBoard);
			blockers = M_RANK::RANK_3 & allBitBoards;
			pawnMoves &= ~(blockers << 8);
			rank = M_RANK::RANK_1 << 8 * getRow(lsb);
			enPassantRank = rank & M_RANK::RANK_5;
			lastPawnMoveStart = MSB_v(getLastPieceOrgDst_bb(bP));
			lastPawnMoveEnd = LSB_v(getLastPieceOrgDst_bb(bP)) & (lastPawnMoveStart >> 16);
			enPasants = enPassantRank & lastPawnMoveEnd & (east(lsb) | west(lsb));
			pawnMoves |= (enPasants << 8);
			pawnMoves |= (wP_Capture_Moves[lsb] & blackBitBoards);
			finalPawnMoves |= pawnMoves;
		}
	}
	else
	{
		pos = bitBoard[bP];
		while (pos)
		{
			lsb = pop_LSB(pos);
			pawnMoves = (bP_Moves[lsb] & emptyBitBoard);
			blockers = M_RANK::RANK_6 & allBitBoards;
			pawnMoves &= ~(blockers >> 8);
			rank = M_RANK::RANK_1 << 8 * getRow(lsb);
			enPassantRank = rank & M_RANK::RANK_4;
			lastPawnMoveStart = MSB_v(getLastPieceOrgDst_bb(wP));
			lastPawnMoveEnd = LSB_v(getLastPieceOrgDst_bb(wP)) & (lastPawnMoveStart << 16);
			enPasants = enPassantRank & lastPawnMoveEnd & (east(lsb) | west(lsb));
			pawnMoves |= (enPasants >> 8);
			pawnMoves |= (bP_Capture_Moves[lsb] & whiteBitBoards);
			finalPawnMoves |= pawnMoves;
		}
	}
	return finalPawnMoves;
}

U64 Board::getPawnCaptureAttacks()
{
	U64 pos;
	U32 lsb;
	U64 pawnAttacks{ 0LL };
	if (wturn)
	{
		pos = bitBoard[wP];
		while (pos)
		{
			lsb = pop_LSB(pos);
			pawnAttacks |= wP_Capture_Moves[lsb];
		}
	}
	else
	{
		pos = bitBoard[bP];
		while (pos)
		{
			lsb = pop_LSB(pos);
			pawnAttacks |= bP_Capture_Moves[lsb];
		}
	}
	return pawnAttacks;
}
U64 Board::getAttacks(int piece)
{
	return (this->*CaptureAttackFunctions[piece])();
}

/*
* comments in generateRookMoves;
* //U32 row{ lsb / 8 };
			//U32 col{ lsb % 8 };
			//U64 rank{ M_RANK::RANK_1 << (8 * row) };
* //U32 index{ lsb + 1 };
			//U32 loopRow{ row };

			//// towards right
			//loopRow = index / 8;
			//while (loopRow == row)
			//{
			//	if (get_bit(rookMoves, index) & allBitBoards) {
			//		++index;
			//		loopRow = index / 8;
			//		while (loopRow == row) {
			//			clear_bit(rookMoves, index++);
			//			loopRow = index / 8;
			//		}
			//		break;
			//	}
			//	loopRow = index / 8;
			//}



			//U64 blockers{ (R_Moves[lsb] & allBitBoards) };
			//std::cout << "Blockers:\n\n"; printBitBoard(blockers); std::cin.get();  system("cls");
			//U32 row{ lsb / 8 };
			//U32 col{ lsb % 8 };
			//U64 file{ M_FILE::FILE_H << col };
			//std::cout << "FILE:\n\n";  printBitBoard(file); std::cin.get(); system("cls");
			//U64 rank{ M_RANK::RANK_1 << (8 * row) };
			//std::cout << "RANK:\n\n"; printBitBoard(rank); std::cin.get(); system("cls");
			//U64 upperFile{ (file << 32) & blockers };
			//std::cout << "upperFile:\n\n";  printBitBoard(upperFile); std::cin.get(); system("cls");
			//U64 lowerFile{ (file >> 32) & blockers };
			//std::cout << "lowerFile:\n\n"; printBitBoard(lowerFile); std::cin.get(); system("cls");
			//U64 leftRank{ ((rank >> 4) & rank) & blockers };
			//std::cout << "leftRank:\n\n";  printBitBoard(leftRank); std::cin.get(); system("cls");
			//U64 rightRank{ ((rank << 4) & rank) & blockers };
			//std::cout << "RightRank:\n\n";  printBitBoard(rightRank); std::cin.get(); system("cls");

			//U64 up{ MSB_v(upperFile) & blackBitBoards };
			//U64 low{ LSB_v(lowerFile) & blackBitBoards };
			//U64 left{ MSB_v(leftRank) & blackBitBoards };
			//U64 right{ LSB_v(rightRank) & blackBitBoards };

			//std::cout << "up:\n\n";  printBitBoard(up); std::cin.get(); system("cls");
			//std::cout << "low:\n\n";  printBitBoard(low); std::cin.get(); system("cls");
			//std::cout << "left:\n\n";  printBitBoard(left); std::cin.get(); system("cls");
			//std::cout << "right:\n\n";  printBitBoard(right); std::cin.get(); system("cls");

			//rookMoves = ((~blockers) & R_Moves[lsb]); /*| up | low | left | right;*/
//std::cout << "rookMoves:\n\n";  printBitBoard(rookMoves); std::cin.get(); system("cls");

void Board::printCurrentMoves()
{
	if (wturn)
	{
		for (int i{ wP }; i < bP; ++i)
		{
			for (int index{ 0 }; index < 64; ++index)
			{
				if (get_bit(bitBoard[i], index)) 
				{
					std::cout << index << '-';
					
				}
			}
		}
	}
}

void Board::updatePosHis()
{
	++posHisSize;
	U64 tempBit;
	U32 lsb;
	U64 key{ 0ULL };
	for (int i{ wP }; i <= bK; ++i)
	{
		posHis[MAX_MOVES - posHisSize][i] = bitBoard[i];
		tempBit = bitBoard[i];
		while (tempBit)
		{
			lsb = pop_LSB(tempBit);
			key ^= hashPieces[i][lsb];
		}
	}
	castlingHis[MAX_MOVES - posHisSize] = castlingRights;
	for (U32 file{ FILE_A }; file <= FILE_H; ++file)
	{
		key ^= hashEnPassant[file];
	}
	for (U32 castlingRight{ 0 }; castlingRight < 4; ++castlingRight)
	{
		key ^= hashCastlingRights[castlingRight];
	}
	key ^= hashSideToMove;
	hashHis[MAX_MOVES - posHisSize] = key;
	hashKey = key;
}

int Board::getOpPiece(int piece)
{
	return (wturn) ? piece + 6 : piece - 6;
}

inline U32 Board::getOrigin(U32 move)
{
	return move & ORIGIN_SQ_MASK;
}
inline U32 Board::getDest(U32 move) {
	return (move & DEST_SQ_MASK) >> 6; 
}
inline U32 Board::getMoveType(U32 move)
{
	return (move & MOVE_TYPE_MASK) >> 12;
}
inline U32 Board::getPiece(U32 move) { return (move & PIECE_MASK) >> 18; }
inline U32 Board::getPromPiece(U32 move) { return (move & PROM_MASK) >> 24; }

void Board::clearPiece(U32 square)
{
	if (wturn) {
		for (int i{ bP }; i <= bK; ++i)
		{
			clear_bit(bitBoard[i], square);
		}
	}
	else
	{
		for (int i{ wP }; i < bP; ++i)
		{
			clear_bit(bitBoard[i], square);
		}
	}
}

bool Board::isAttacked(U32 square, int attackerColor)
{
	U64 squarePos{ 1ULL << square };
	U64 squareIsAttacked{ 0 };
	U64 pos;
	U64 lsb;
	for (int i{ bP - attackerColor * 6 }; i <= bK - attackerColor * 6; ++i)
	{
		pos = bitBoard[i];
		while (pos)
		{
			lsb = pop_LSB(pos);
			squareIsAttacked = (All_Moves[i][lsb] & squarePos);
			if (squareIsAttacked)
			{
				bool turn{ wturn };
				wturn = attackerColor;
				squareIsAttacked = (getAttacks(i) & squarePos);
				wturn = turn;
				if (squareIsAttacked)
					return true;
			}
		}
	}
	return false;
}

bool Board::isLegal(U32 move)
{
	playMove(move);
	if (bitBoard[wK] == 0 || bitBoard[bK] == 0)
	{
		undoMove(move);
		return false;
	}
	/*system("cls");
	printBoard();*/
	int turn{ (int)(!wturn) };
	U64 kingPos{ bitBoard[bK - turn * 6] };
	U64 kingIsAttacked{ 0 };
	U64 pos;
	U64 lsb;
	for (int i{ bP - (int)wturn * 6 }; i <= bK - wturn * 6; ++i)
	{
		pos = bitBoard[i];
		while (pos)
		{
			lsb = pop_LSB(pos);
			kingIsAttacked = (All_Moves[i][lsb] & kingPos);
			if (kingIsAttacked)
			{
				kingIsAttacked = (getAttacks(i) & kingPos);
				if (kingIsAttacked)
				{
					undoMove(move);
					return false;
				}
			}
		}
	}
	undoMove(move);
	return true;
}

void Board::playMove(U32 move)
{
	U32 origin{ move & ORIGIN_SQ_MASK };
	U32 dest{ (move & DEST_SQ_MASK) >> 6 };
	U32 moveType{ (move & MOVE_TYPE_MASK) >> 12 };
	U32 piece{ (move & PIECE_MASK) >> 18 };
	U32 promPiece{ (move & PROM_MASK) >> 24 };
	
	clear_bit(bitBoard[piece], origin);
	set_bit(bitBoard[piece], dest);
	//printBoard();
	if (piece == wK)
	{
		castlingRights &= (~M_RANK::RANK_1);
	}
	else if (piece == bK)
	{
		castlingRights &= (~M_RANK::RANK_8);
	}
	else if (piece == wR)
	{
		castlingRights &= (~(((1ULL << H1) & (1ULL << origin)) << 1));
		castlingRights &= (~(((1ULL << A1) & (1ULL << origin)) >> 2));
	}
	else if (piece == bR)
	{
		castlingRights &= (~(((1ULL << H8) & (1ULL << origin)) << 1));
		castlingRights &= (~(((1ULL << A8) & (1ULL << origin)) >> 2));
	}

	if (get_bit(moveType, quiet))
	{
		updatePosHis();
		updateBitBoards();
		wturn = !wturn;
		return;
	}
	if (get_bit(moveType, castling))
	{
		int rookSq = (origin) + ((dest - origin) / 2);
		U32 rook = piece - 2;
		set_bit(bitBoard[rook], rookSq);
		U64 deleteRook{ ((1ULL << (rookSq - 2)) & M_FILE::FILE_H) | ((1ULL << (rookSq + 3)) & M_FILE::FILE_A) };
		//_printBitBoard(deleteRook);
		//deleteRook &= bitBoard[rook];
		clear_bit(bitBoard[rook], LSB(deleteRook));
		U64 rank{ M_RANK::RANK_1 << (getRow(origin) * 8) };
		castlingRights &= (~rank);
		updatePosHis();
		updateBitBoards();
		wturn = !wturn;
		return;
	}
	

	if (get_bit(moveType, capture))
	{
		clear_bit(moveType, enpasant);
		clearPiece(dest);
		//clear_bit(bitBoard[piece], dest);

		if (dest == H1)
			castlingRights &= (~(1ULL << G1));
		else if (dest == A1)
			castlingRights &= (~(1ULL << C1));
		else if (dest == H8)
			castlingRights &= (~(1ULL << G8));
		else if (dest == A8)
			castlingRights &= (~(1ULL << C8));

		/*castlingRights &= (~(((1ULL << H1) & (1ULL << dest)) << 2));
		castlingRights &= (~(((1ULL << A1) & (1ULL << dest)) >> 2));
		castlingRights &= (~(((1ULL << H8) & (1ULL << dest)) << 2));
		castlingRights &= (~(((1ULL << A8) & (1ULL << dest)) >> 2));*/
	}
	if (get_bit(moveType, promotion))
	{
		clear_bit(bitBoard[piece], dest);
		set_bit(bitBoard[promPiece], dest);
	}
	if (get_bit(moveType, enpasant) )
	{
		/*_printBitBoard(bitBoard[getOpPiece(piece)]);
		_printBitBoard(~getLastPieceOrgDst_bb(getOpPiece(piece)));*/
		bitBoard[getOpPiece(piece)] &= (~getLastPieceOrgDst_bb(getOpPiece(piece)));
		//printBoard();
	}
	//totalMoves = 0;
	updatePosHis();
	updateBitBoards();
	wturn = !wturn;
}
void Board::undoMove(U32 move)
{
	move;
	--posHisSize;
	for (int i{ wP }; i <= bK; ++i)
	{
		bitBoard[i] = posHis[MAX_MOVES - posHisSize][i];
	}
	castlingRights = castlingHis[MAX_MOVES - posHisSize];
	hashKey = hashHis[MAX_MOVES - posHisSize];
	updateBitBoards();
	wturn = !wturn;
	/*system("cls");
	printBoard();*/
	/*U32 origin{ (move & ORIGIN_SQ_MASK) };
	U32 dest{ (move & DEST_SQ_MASK) >> 6 };
	U32 moveType{ (move & MOVE_TYPE_MASK) >> 12 };
	U32 piece{ (move & PIECE_MASK) >> 18 };
	U32 promPiece{ (move & PROM_MASK) >> 24 };

	set_bit(bitBoard[piece], origin);
	clear_bit(bitBoard[piece], dest);
	if (get_bit(moveType, capture))
	{
		for (int i{ wP }; i <= bK; ++i) {
			if (get_bit(posHis[MAX_MOVES - posHisSize][i], dest))
				set_bit(posHis[MAX_MOVES - posHisSize][i], dest);
		}
			set_bit(posHis[MAX_MOVES - ], dest);
	}
	if (get_bit(moveType, promotion))
	{
		clear_bit(bitBoard[])
	}*/
	// -- posHisSize; // I might want to add this later.
}

void Board::redoMove()
{
	++posHisSize;
	for (int i{ wP }; i <= bK; ++i)
	{
		bitBoard[i] = posHis[MAX_MOVES - posHisSize][i];
	}
	castlingRights = castlingHis[MAX_MOVES - posHisSize];
	hashKey = hashHis[MAX_MOVES - posHisSize];
	updateBitBoards();
	wturn = !wturn;
}

void Board::printMoves(U32* list, U32 moves)
{
	printBoard();
	std::cin.get();
	system("cls");
	for (U32 i{ 0 }; i < moves; ++i)
	{
		playMove(list[i]);
		printBoard();
		std::cin.get();
		system("cls");
		undoMove(list[i]);
	}
}


void Board::_printBitBoard(U64 b, const char* msg)
{
	std::cout << msg << ":\n\n";  
	printBitBoard(b); 
	std::cin.get(); 
	system("cls");
}

void Board::_add_piece(int piece, int sq)
{
	set_bit(bitBoard[piece], sq);
}
void Board::_remove_piece(int piece, int sq)
{
	clear_bit(bitBoard[piece], sq);
}

void Board::_clearBoard_()
{
	for (int i{ wP }; i <= bK; ++i)
	{
		bitBoard[i] = 0;
		//_printBitBoard(bitBoard[i]);
	}
	whiteBitBoards = 0;
	blackBitBoards = 0;
	emptyBitBoard = 0;
	allBitBoards = 0;
	castlingRights = 0;
	castlingHis[MAX_MOVES - posHisSize] = 0;
	wturn = true;
	for (int i{ wP }; i <= bK; ++i)
	{
		posHis[MAX_MOVES - posHisSize][i] = bitBoard[i];
	}
	//posHisSize = 0;
}

inline bool Board::isBtwn(int value, int l, int r)
{
	return !(value < l || value > r);
}
inline bool Board::isValidLetter(char letter)
{
	return isBtwn(letter, 'a', 'h');
}
inline bool Board::isValidNumber(char number){ return isBtwn(number, '1', '8'); }

U32 Board::getPromotionPiece()
{
#define PRINT_MENU \
	std::cout << "Promotion piece:\n";  \
	std::cout << "1. Knight\n";         \
	std::cout << "2. Bishop\n";			\
	std::cout << "3. Rook\n";			\
	std::cout << "4. Queen\n";			

	PRINT_MENU;
	std::string promPiece;
	std::getline(std::cin, promPiece);
	while (promPiece.size() != 1 && (promPiece[0] < '1' || promPiece[0] > '4')) {
		system("cls");
		std::cout << "Invalid input, try again\n";
		PRINT_MENU;
		std::getline(std::cin, promPiece);
	}
#undef PRINT_MENU
	return wturn ? (promPiece[0] - '0') : (promPiece[0] - '0' + 6);
}

bool Board::isPieceMove(ChessMove move, U32* list, U32 size)
{
	for (U32 i{ 0 }; i < size; ++i)
	{
		if (move.endSquare == getDest(list[i]) && move.startSquare == getOrigin(list[i])) {
			if (get_bit( getMoveType(list[i]), promotion ) )
			{
				U32 promPiece = getPromotionPiece();
				while (i < size && move.endSquare == getDest(list[i]) && move.startSquare == getOrigin(list[i]))
				{
					if (getPromPiece(list[i]) == promPiece) {
						playerMove = list[i];
						return true;
					}
					++i;
					//_printBitBoard(list[i]);
				}
			}
			playerMove = list[i];
			return true;
		}
	}
	return false;
}

bool Board::isValidMove(const std::string& move)
{
	ChessMove cm{ convert(move) };
	return
		isPieceMove(cm, P_list, totalPawnMoves) ||
		isPieceMove(cm, Prom_list, totalPromMoves) ||
		isPieceMove(cm, N_list, totalKnightMoves) ||
		isPieceMove(cm, R_list, totalRookMoves) ||
		isPieceMove(cm, B_list, totalBishopMoves) ||
		isPieceMove(cm, Q_list, totalQueenMoves) ||
		isPieceMove(cm, K_list, totalKingMoves);
}

bool Board::isValidMove(ChessMove cm)
{
	return
		isPieceMove(cm, P_list, totalPawnMoves) ||
		isPieceMove(cm, Prom_list, totalPromMoves) ||
		isPieceMove(cm, N_list, totalKnightMoves) ||
		isPieceMove(cm, R_list, totalRookMoves) ||
		isPieceMove(cm, B_list, totalBishopMoves) ||
		isPieceMove(cm, Q_list, totalQueenMoves) ||
		isPieceMove(cm, K_list, totalKingMoves);
}

std::string Board::getMove()
{
	std::string move;
	while (true)
	{
		std::cout << "Enter your move: ";
		std::getline(std::cin, move);
		/*bool exitCondition{ !(move.size() != 4 || !isBtwn(move[0], 'a', 'h') || !isBtwn(move[2], 'a', 'h') ||
			!isBtwn(move[1], '1', '8') || !isBtwn(move[3], '1', '8'))};*/
		bool exit{
			!(
			move.size() != 4 ||
			!isValidLetter(move[0]) ||
			!isValidLetter(move[2]) ||
			!isValidNumber(move[1]) ||
			!isValidNumber(move[3]) ||
			!isValidMove(move))
		};
		if (exit)
			break;
		/*if (move.size() != 4)
			continue;
		if (move[0] < 'a' || move[0] > 'h')
			continue;
		if (move[2] < 'a' || move[2] > 'h')
			continue;
		if (move[1] < '1' || move[1] > '8')
			continue;
		if (move[3] < '1' || move[3] > '8')
			continue;
		break;*/
	}
	return move;
}

inline U32 Board::squareOf(U32 row, U32 col)
{
	return row * 8 + col;
}
inline RowCol_s Board::getRowCol(U32 square)
{
	return { (square / 8), (square % 8) };
}
inline U32 Board::getRow(U32 square)
{
	return square / 8;
}
inline U32 Board::getCol(U32 square)
{
	return square % 8;
}

ChessMove Board::convert(const std::string& str)
{
	std::string startSquare{ str.begin(), str.begin() + 2 };
	std::string endSquare{ str.begin() + 2, str.end() };
	/*std::cout << "startSquare: " << startSquare << '\n';
	std::cout << "endSquare: " << endSquare << '\n';*/
	
	U32 start_row{ getRow(startSquare[1]) };
	U32 start_col{ getCol(startSquare[0]) };

	U32 end_row{ getRow(endSquare[1]) };
	U32 end_col{ getCol(endSquare[0]) };

	ChessMove res{ squareOf(start_row, start_col), squareOf(end_row, end_col) };
	return res;
}

void Board::_show_move(U32 move)
{
	playMove(move);
	system("cls");
	printBoard();
	undoMove(move);
}

void Board::_go_through_the_moves()
{
	std::cout << "going through moves...(press enter)";
	std::cin.get();
	for (U32 i{ 0 }; i < totalPawnMoves; ++i)
	{
		_show_move(P_list[i]);
		std::cin.get();
	}
	for (U32 i{ 0 }; i < totalPromMoves; ++i)
	{
		_show_move(Prom_list[i]);
		std::cin.get();
	}
	for (U32 i{ 0 }; i < totalKnightMoves; ++i)
	{
		_show_move(N_list[i]);
		std::cin.get();
	}
	for (U32 i{ 0 }; i < totalBishopMoves; ++i)
	{
		_show_move(B_list[i]);
		std::cin.get();
	}
	for (U32 i{ 0 }; i < totalRookMoves; ++i)
	{
		_show_move(R_list[i]);
		std::cin.get();
	}
	for (U32 i{ 0 }; i < totalQueenMoves; ++i)
	{
		_show_move(Q_list[i]);
		std::cin.get();
	}
	for (U32 i{ 0 }; i < totalKingMoves; ++i)
	{
		_show_move(K_list[i]);
		std::cin.get();
	}
	system("cls");
	printBoard();
}

U32 Board::totalMoves()
{
	(totalAllMoves =
		totalPawnMoves + totalPromMoves + totalKnightMoves + totalBishopMoves + totalRookMoves + totalQueenMoves + totalKingMoves);
	return totalAllMoves;
}

Result Board::getResult()
{
	if (totalMoves() == 0) {
		int king{ bK - wturn * 6 };
		if (isAttacked(LSB(bitBoard[king]), !wturn))
			return static_cast<Result>(!wturn);
		return DRAW;
	}
	U32 threeFoldCount{ 1 };
	for (U32 i{ MAX_MOVES - 1 }; i > MAX_MOVES - posHisSize; --i)
	{
		if (hashHis[i] == hashKey)
			++threeFoldCount;
		if (threeFoldCount == 3)
			return DRAW;
	}
	return PLAYING;
}

void Board::swapIfCapture(U32& captureMove, U32& swapMove, U32& advanceIndex)
{
	U32 tempMove;
	if (get_bit(captureMove, capture))
	{
		tempMove = captureMove;
		captureMove = swapMove;
		swapMove = tempMove;
		++advanceIndex;
	}
}
void Board::swap(U32& move1, U32& move2)
{
	U32 temp{ move1 };
	move1 = move2;
	move2 = temp;
}

void Board::copyMoves(U32* avMoves)
{

	U32 current{ 0 };
	
	U32 start{ 0 };
	
	for (U32 i{ totalPromMoves }; i > 0;  --i)
	{
		avMoves[current++] = Prom_list[i-1];
		++start;
	}
	for (U32 i{ 0 }; i < totalQueenMoves; ++i)
	{
		avMoves[current++] = Q_list[i];
		swapIfCapture(avMoves[current - 1], avMoves[start], start);
	}
	for (U32 i{ 0 }; i < totalRookMoves; ++i)
	{
		avMoves[current++] = R_list[i];
		swapIfCapture(avMoves[current - 1], avMoves[start], start);
	}
	for (U32 i{ 0 }; i < totalBishopMoves; ++i)
	{
		avMoves[current++] = B_list[i];
		swapIfCapture(avMoves[current - 1], avMoves[start], start);
	}
	for (U32 i{ 0 }; i < totalKnightMoves; ++i)
	{
		avMoves[current++] = N_list[i];
		swapIfCapture(avMoves[current - 1], avMoves[start], start);
	}
	for (U32 i{ 0 }; i < totalPawnMoves; ++i)
	{
		avMoves[current++] = P_list[i];
		swapIfCapture(avMoves[current - 1], avMoves[start], start);
	}
	for (U32 i{ 0 }; i < totalKingMoves; ++i)
	{
		avMoves[current++] = K_list[i];
		swapIfCapture(avMoves[current - 1], avMoves[start], start);
	}
	//assert(current == totalAllMoves);
}

U32 Board::popcnt(U64 b)
{
	U32 sum{ 0 };
	while (b)
	{
		b &= (b - 1); // clear last bit
		++sum;
	}
	return sum;
}

void Board::initPosValues()
{
	U32 row;
	U32 col;
	for (U32 i{ 0 }; i < 64; ++i)
	{
		row = getRow(i);
		col = getCol(i);
		POS_VALUE[wP][i] = 5*(row) + colValPawn(col);
		POS_VALUE[bP][i] = -(5*(7 - (int)row) + colValPawn(col));



		if (row == 6)
		{
			POS_VALUE[wQ][i] = 20;
			POS_VALUE[wR][i] = 20;
		}
		else if (row == 1)
		{
			POS_VALUE[bQ][i] = -20;
			POS_VALUE[bR][i] = -20;
		}
		if (row == 0 || row == 7)
		{
			POS_VALUE[wN][i] = -20;
			POS_VALUE[wB][i] = -15;
			POS_VALUE[bN][i] = 20;
			POS_VALUE[bB][i] = 15;

			POS_VALUE[wQ][i] = 50;
			POS_VALUE[bQ][i] = -50;
		}
		else if (i == E2 || i == D2)
			POS_VALUE[wP][i] = -30;
		else if (i == E7 || i == D7)
			POS_VALUE[bP][i] = 30;
		else if (i == E4 || i == E5 || i == D4 || i == D5)
		{
			POS_VALUE[wN][i] = 5;
			POS_VALUE[wB][i] = 2;
			POS_VALUE[bN][i] = -5;
			POS_VALUE[bB][i] = -2;
			POS_VALUE[wQ][i] = -10;
			POS_VALUE[bQ][i] = 10;

			POS_VALUE[wP][i] = 50;
			POS_VALUE[bP][i] = -50;
		}
		else if (col == 0 || col == 7)
		{
			POS_VALUE[wN][i] = -20;
			POS_VALUE[bN][i] = 20;
			POS_VALUE[bB][i] = 15;
			POS_VALUE[wB][i] = -15;
		}
		
	}
}

int Board::colValPawn(U32 col)
{
	if (col == 1)
		return 1;
	if (col == 2)
		return 2;
	if (col == 3)
		return 4;
	if (col == 4)
		return 4;
	if (col == 5)
		return 2;
	if (col == 6)
		return 1;
	return 0;
}

int Board::posValue(int piece, U64 b)
{
	U32 lsb;
	int sum{ 0 };
	while (b)
	{
		lsb = pop_LSB(b);
		sum += POS_VALUE[piece][lsb];
	}
	return sum;
}

int Board::eval()
{
	int sum{ 0 };
	int sign{ wturn ? 1 : -1 };
	/*U32 ourMoves{ totalAllMoves };
	U32 theirMoves;*/
	
	for (int i{ wP }; i <= bK; ++i)
	{
		sum += (PIECE_VALUE[i] * popcnt(bitBoard[i]) + posValue(i, bitBoard[i]));
	}
	if (isAttacked(LSB(bitBoard[bK - (!wturn) * 6]), wturn)) {
		sum += (-sign) * 5;
		/*if (isAttacked(LSB(bitBoard[bK - (!wturn) * 6]), wturn)) {
			sum += (-sign) * 450;
		}*/
	}
	/*wturn = !wturn;
	generateAllMoves();
	wturn = !wturn;
	theirMoves = totalMoves();
	sum = sum + (sign)*ourMoves - (sign)*theirMoves;*/
	
	return sum + (wturn? totalAllMoves/2 : -(int)totalAllMoves/2);
}

MoveScore_s Board::computerMoveTh(U32 depth)
{
	generateAllMoves();
	MoveScore_s ms{ wturn ? MoveScore_s{FAKE_MOVE, -2 * WIN_SCORE} : MoveScore_s{FAKE_MOVE, 2 * WIN_SCORE} };
	MoveScore_s temp[218]{};
	MoveScore_s bestTemp{};
	
	U32 avMoves[218];
	copyMoves(avMoves);
	U64 totals{ totalAllMoves };
	//char threadsMem[6 * sizeof(std::thread{})];
	std::thread threads[6];
	/*for (int i{ 0 }; i < 6; ++i)
	{
		threads[i] = std::thread(&Board::doNothing, this);
	}*/
	bool isActive[6]{ false, false, false, false, false, false };
	bool isDead[6]{ false, false, false, false, false, false };

	int curr{ 0 };
	bool moveIsAsigned;
	/*char b[6 * sizeof(Board)];
	Board* ptr = (Board*)(b);*/

	bool finished{ false };
	for (U32 i{ 0 }; i < totals; ++i)
	{
		playMove(avMoves[i]);
		moveIsAsigned = false;
		while (!moveIsAsigned)
		{
			for (int j{ 0 }; j < 6; ++j)
			{
				if (!isActive[j]) {
					moveIsAsigned = true;
					if (isDead[j]) {
						threads[j].join();
						threads[j].~thread();
						new (threads + j) std::thread();
						isDead[j] = false;
					}
					temp[curr].move = avMoves[i];
					isActive[j] = true;
				threads[j] = std::thread(&Board::compMove, this, *this,
					depth - 1, &temp[curr++], isActive + j, isDead + j, &finished);
					break;
				}
			}
			bestTemp = wturn ? min(temp, curr) : max(temp, curr);
			if ((bestTemp.score == WIN_SCORE && !wturn) || (bestTemp.score == -WIN_SCORE && wturn)) {
				finished = true;
				undoMove(avMoves[i]);
				for (int k{ 0 }; k < 6; ++i)
				{
					if (threads[k].joinable())
						threads[k].join();
				}
				return bestTemp;
			}
		}
		undoMove(avMoves[i]);
	}

	bestTemp = wturn ? max(temp, totals) : min(temp, totals);
	if ((bestTemp.score == WIN_SCORE && wturn) || (bestTemp.score == -WIN_SCORE && !wturn)) {
		finished = true;
		return bestTemp;
	}
	for (int i{ 0 }; i < 6; ++i)
	{
		if (threads[i].joinable())
			threads[i].join();
	}
	bestTemp = wturn ? max(temp, totals) : min(temp, totals);
	return bestTemp;
}


void Board::compMove(Board b, U32 depth, MoveScore_s* ms, bool* isActive, bool* isDead, bool* finished)
{
	ms->score = b.computerMove(depth, INT32_MIN, INT32_MAX, finished).score;
	*isDead = true;
	*isActive = false;
	/*if ((!wturn && ms->score == WIN_SCORE) || (wturn && ms->score == -WIN_SCORE))
	{
		*finished = true;
		return;
	}*/
}

MoveScore_s Board::quietSearch(U32 depth, bool* exit)
{
	if (exit && *exit)
		return { FAKE_MOVE, FAKE_SCORE };
	generateAllMoves();
	Result res{ getResult() };
	if (depth == 0)
	{
		if (res == PLAYING)
			return { FAKE_MOVE, eval() };
		else if (res == WHITE_WIN)
			return { FAKE_MOVE, WIN_SCORE };
		else if (res == BLACK_WIN)
			return { FAKE_MOVE, -WIN_SCORE };
		else
			return{ FAKE_MOVE, DRAW_SCORE };
	}
	if (res == PLAYING)
	{
		MoveScore_s ms{ wturn ? MoveScore_s{FAKE_MOVE, -2 * WIN_SCORE} : MoveScore_s{FAKE_MOVE, 2 * WIN_SCORE} };
		MoveScore_s temp;
		U32 avMoves[218]; 
		copyMoves(avMoves);
		U64 totals{ totalAllMoves };
		U32 moveType;
		bool isCapture;
		for (U32 i{ 0 }; i < totals; ++i)
		{
			moveType = ((avMoves[i] & MOVE_TYPE_MASK) >> 12);
			isCapture = (bool)get_bit(moveType, capture);
			if (!isCapture)
				continue;
			playMove(avMoves[i]);
			temp = quietSearch(depth - 1, exit);
			undoMove(avMoves[i]);
			if (exit && *exit)
				return { FAKE_MOVE, FAKE_SCORE };
			if (wturn && temp.score > ms.score) {
				ms = { avMoves[i], temp.score };
				if (ms.score >= WIN_SCORE)
					return ms;
			}
			else if (!wturn && temp.score < ms.score) {
				ms = { avMoves[i], temp.score };
				if (ms.score <= -WIN_SCORE)
					return ms;
			}
		}
		if (ms.move == FAKE_MOVE)
			return { FAKE_MOVE, eval() };
		return ms;
	}
	else if (res == WHITE_WIN)
		return { FAKE_MOVE, WIN_SCORE };
	else if (res == BLACK_WIN)
		return { FAKE_MOVE, -WIN_SCORE };
	else
		return{ FAKE_MOVE, DRAW_SCORE };

}

MoveScore_s Board::quietSearchSq(U32 depth, U32 move, bool* exit)
{
	if (exit && *exit)
		return { FAKE_MOVE, FAKE_SCORE };
	generateAllMoves();
	Result res{ getResult() };
	if (depth == 0)
	{
		if (res == PLAYING)
			return { FAKE_MOVE, eval() };
		else if (res == WHITE_WIN)
			return { FAKE_MOVE, WIN_SCORE };
		else if (res == BLACK_WIN)
			return { FAKE_MOVE, -WIN_SCORE };
		else
			return{ FAKE_MOVE, DRAW_SCORE };
	}
	if (res == PLAYING)
	{
		MoveScore_s ms{ wturn ? MoveScore_s{FAKE_MOVE, -2 * WIN_SCORE} : MoveScore_s{FAKE_MOVE, 2 * WIN_SCORE} };
		MoveScore_s temp;
		U32 avMoves[218];
		copyMoves(avMoves);
		U64 totals{ totalAllMoves };
		
		for (U32 i{ 0 }; i < totals; ++i)
		{
			if (getDest(avMoves[i]) != getDest(move))
				continue;
			playMove(avMoves[i]);
			temp = quietSearchSq(depth - 1, avMoves[i], exit);
			undoMove(avMoves[i]);
			if (exit && *exit)
				return { FAKE_MOVE, FAKE_SCORE };
			if (wturn && temp.score > ms.score) {
				ms = { avMoves[i], temp.score };
				if (ms.score >= WIN_SCORE)
					return ms;
			}
			else if (!wturn && temp.score < ms.score) {
				ms = { avMoves[i], temp.score };
				if (ms.score <= -WIN_SCORE)
					return ms;
			}
		}
		if (ms.move == FAKE_MOVE)
			return { FAKE_MOVE, eval() };
		return ms;
	}
	else if (res == WHITE_WIN)
		return { FAKE_MOVE, WIN_SCORE };
	else if (res == BLACK_WIN)
		return { FAKE_MOVE, -WIN_SCORE };
	else
		return{ FAKE_MOVE, DRAW_SCORE };

}

MoveScore_s Board::computerMove(U32 depth, int a, int b, bool* exit) //, int a, int b)
{
	if (exit && *exit)
		return { FAKE_MOVE, FAKE_SCORE };
	generateAllMoves();
	Result res{ getResult() };
	if (depth == 0 ) {
		if(res == PLAYING)
			return { FAKE_MOVE, eval() };
		else if (res == WHITE_WIN)
			return { FAKE_MOVE, WIN_SCORE };
		else if (res == BLACK_WIN)
			return { FAKE_MOVE, -WIN_SCORE };
		else
			return{ FAKE_MOVE, DRAW_SCORE };
		
	}
	if (res == PLAYING)
	{
		MoveScore_s ms{ wturn ? MoveScore_s{FAKE_MOVE, -2 * WIN_SCORE} : MoveScore_s{FAKE_MOVE, 2 * WIN_SCORE} };
		MoveScore_s temp;
		U32 avMoves[218]; 
		copyMoves(avMoves);
		U64 totals{ totalAllMoves };
		U32 moveType;

		for (U32 i{ 0 }; i < totals; ++i)
		{
			moveType = ((avMoves[i] & MOVE_TYPE_MASK) >> 12);
			playMove(avMoves[i]);
			
			if (depth == 1 && get_bit(moveType, capture))
				temp = quietSearchSq(100, avMoves[i], exit);
			else
				temp = computerMove(depth - 1);
			//temp = computerMove(depth - 1);
			undoMove(avMoves[i]);
			if (exit && *exit)
				return { FAKE_MOVE, FAKE_SCORE };
			if (wturn)
			{
				if (temp.score >= b)
					return { avMoves[i], b };
				if ((temp.score > ms.score))
				{
					ms = { avMoves[i], temp.score };
					if (ms.score >= WIN_SCORE)
						return ms;
				}
				if (temp.score > a)
				{
					a = ms.score;
				}
				
			}
			else
			{
				if (temp.score <= a)
					return { avMoves[i], a };
				if ((temp.score < ms.score))
				{
					ms = { avMoves[i], temp.score };
					if (ms.score <= -WIN_SCORE)
						return ms;
				}
				if (temp.score < b)
				{
					b = ms.score;
				}
			}
			if (b <= a)
				return ms;
		}
		return ms;
	}
	else if (res == WHITE_WIN)
		return { FAKE_MOVE, WIN_SCORE };
	else if (res == BLACK_WIN)
		return { FAKE_MOVE, -WIN_SCORE };
	else
		return{ FAKE_MOVE, DRAW_SCORE };
}

void Board::perf(U32 depth) //, int a, int b)
{
	generateAllMoves();
	U32 totals{ totalMoves() };
	//Result res{ getResult() };

	/*printBoard();
	_go_through_the_moves();*/
	if (depth == 1) {
		perfMoves += totals;
		return;
	}
	else if (depth == 0) {
		++perfMoves;
		return;
	}
		/*if (res == PLAYING)
			return { FAKE_MOVE, eval() };
		else if (res == WHITE_WIN)
			return { FAKE_MOVE, WIN_SCORE };
		else if (res == BLACK_WIN)
			return { FAKE_MOVE, -WIN_SCORE };
		else
			return{ FAKE_MOVE, DRAW_SCORE };*/
		
	
	//if (res == PLAYING)
	{
		//MoveScore_s ms{ wturn ? MoveScore_s{FAKE_MOVE, -2 * WIN_SCORE} : MoveScore_s{FAKE_MOVE, 2 * WIN_SCORE} };
		//MoveScore_s temp;
		U32 avMoves[218];
		copyMoves(avMoves);
		//perfMoves += totals;
		for (U32 i{ 0 }; i < totals; ++i)
		{
			playMove(avMoves[i]);
			
			//temp = perf(depth - 1);
			perf(depth - 1);
			undoMove(avMoves[i]);
			
			/*if (wturn)
			{
				if ((temp.score > ms.score))
				{
					ms = { avMoves[i], temp.score };
				}
			}
			else
			{
				if ((temp.score < ms.score))
				{
					ms = { avMoves[i], temp.score };
				}
			}*/
		}
		//return ms;
	}
	/*else if (res == WHITE_WIN)
		return { FAKE_MOVE, WIN_SCORE };
	else if (res == BLACK_WIN)
		return { FAKE_MOVE, -WIN_SCORE };
	else
		return{ FAKE_MOVE, DRAW_SCORE };*/
}

void Board::perftFen(Pos* pos, bool* isThreadActive)
{
	Board board;
	board.parseFen(pos->fen);
	board.perfPerMoveNoPrint(*pos);
	*isThreadActive = false;
	//std::cout << "One of the threads ended\n";
}

void Board::perfTestsTh()
{
	Pos posFens[]
	{
		{"Position 1", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324}, //6
		{"Position 2", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 5, 193690690}, //5
		{"Position 3", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 7, 178633661}, //7
		{"Position 4", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, 706045033}, //6
		{"Position 5", "1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1", 5, 1063513} //5
	};
	constexpr int fenNum = (int)arraySize(posFens);
	constexpr int THREADS{ fenNum >= 6 ? 6 : fenNum };
	int currentFen{ 0 };
	std::thread thread[THREADS];
	bool isActive[THREADS]{ false, false, false, false, false };
	while(currentFen < fenNum)
	{
		for (int i{ 0 }; i < THREADS; ++i)
		{
			if (!isActive[i] && currentFen < fenNum)
			{
				isActive[i] = true;
				thread[i] = std::thread(&Board::perftFen, this, &(posFens[currentFen++]), (isActive + i));
				//std::cout << "starting thread " << i << '\n';
			}
		}
	}
	for (int i{ 0 }; i < THREADS; ++i) {
		thread[i].join();
	}
	//std::cout << posFens << '\n';
	std::cout << posFens << '\n';
}
void Board::performanceTests()
{
	Board board;

	board.parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	board.perfPerMove(6);
	if (board.perfMoves == 119060324)
		std::cout << "  (correct result)\n\n\n";
	else
		std::cout << "  xx(incorrect result)xx\n\n\n";
	board.perfMoves = 0;

	board.parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
	board.perfPerMove(5);
	if (board.perfMoves == 193690690)
		std::cout << "  (correct result)\n\n\n";
	else
		std::cout << "  xx(incorrect result)xx\n\n\n";
	board.perfMoves = 0;

	board.parseFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
	board.perfPerMove(7);
	if (board.perfMoves == 178633661)
		std::cout << "  (correct result)\n\n\n";
	else
		std::cout << "  xx(incorrect result)xx\n\n\n";
	board.perfMoves = 0;

	board.parseFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	board.perfPerMove(6);
	if (board.perfMoves == 706045033)
		std::cout << "  (correct result)\n\n\n";
	else
		std::cout << "  xx(incorrect result)xx\n\n\n";
	board.perfMoves = 0;

	board.parseFen("1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1");
	board.perfPerMove(5);
	if (board.perfMoves == 1063513)
		std::cout << "  (correct result)\n\n\n";
	else
		std::cout << "  xx(incorrect result)xx\n\n\n";
	board.perfMoves = 0;
}

void Board::perfPerMoveNoPrint(Pos& pos)
{
	generateAllMoves();
	U32 avMoves[218];
	copyMoves(avMoves);
	U32 totals{ totalMoves() };
	U32 origin, dest;
	U64 endTotals{ 0 };
	//perfMoves += totals;
	U32 current{ 0 };
	for (U32 i{ 0 }; i < totals; ++i)
	{

		playMove(avMoves[i]);
		origin = getOrigin(avMoves[i]);
		dest = getDest(avMoves[i]);
		perf(pos.depth - 1);
		//str += CONVERT_M_str(origin) + CONVERT_M_str(dest) + ": " + std::to_string(perfMoves) + '\n';
		pos.move[current++] = CONVERT_M(origin);
		pos.move[current++] = CONVERT_M(dest);
		pos.out[i] = perfMoves;
		endTotals += perfMoves;
		perfMoves = 0;
		undoMove(avMoves[i]);
	}
	//str += "Total: " + std::to_string(endTotals) + '\n';
	perfMoves = endTotals;
	pos.totalPerfMoves = endTotals;
	pos.totalAvMoves = totals;
	pos.perMove = true;
}

void Board::perfPerMove(U32 depth)
{
	generateAllMoves();
	U32 avMoves[218];
	copyMoves(avMoves);
	U32 totals{ totalMoves() };
	U32 origin, dest;
	U64 endTotals{ 0 };
	//perfMoves += totals;
	for (U32 i{ 0 }; i < totals; ++i)
	{
		
		playMove(avMoves[i]);
		origin = getOrigin(avMoves[i]);
		dest = getDest(avMoves[i]);
		perf(depth - 1);
		std::cout << CONVERT_M(origin) << CONVERT_M(dest) << ": " << perfMoves << '\n';
		endTotals += perfMoves;
		perfMoves = 0;
		undoMove(avMoves[i]);
	}
	std::cout << "Total: " << endTotals << '\n';
	perfMoves = endTotals;
}

void Board::playGame()
{
	//MoveScore_s ms;
	while (true) {
		printBoard();
		//std::cout << "\n\n\n";
		//generateAllMoves();
		//ms = computerMove(3);
		//playMove(ms.move);
		//printBoard();
		//std::cout << "\nEval: " << ms.score << '\n';
		//_go_through_the_moves();
		generateAllMoves();
		ChessMove m{ convert(getMove()) };
		playMove(playerMove);
		//updateBitBoards();
		system("cls");
	}
}