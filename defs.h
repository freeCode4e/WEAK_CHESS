#pragma once
#include "types.h"
#include "MoveScore_s.h"

enum SQUARES
{
	H1,G1,F1,E1,D1,C1,B1,A1, 
	H2,G2,F2,E2,D2,C2,B2,A2,  
	H3,G3,F3,E3,D3,C3,B3,A3,
	H4,G4,F4,E4,D4,C4,B4,A4,
	H5,G5,F5,E5,D5,C5,B5,A5,
	H6,G6,F6,E6,D6,C6,B6,A6,
	H7,G7,F7,E7,D7,C7,B7,A7,
	H8,G8,F8,E8,D8,C8,B8,A8
};

constexpr const char* SQUARES_TO_STRING[64]
{
	"H1","G1","F1","E1","D1","C1","B1","A1",
	"H2","G2","F2","E2","D2","C2","B2","A2",
	"H3","G3","F3","E3","D3","C3","B3","A3",
	"H4","G4","F4","E4","D4","C4","B4","A4",
	"H5","G5","F5","E5","D5","C5","B5","A5",
	"H6","G6","F6","E6","D6","C6","B6","A6",
	"H7","G7","F7","E7","D7","C7","B7","A7",
	"H8","G8","F8","E8","D8","C8","B8","A8"
};

#define CONVERT_M(sq) SQUARES_TO_STRING[sq]
#define CONVERT_M_str(sq) std::string{SQUARES_TO_STRING[sq]}

namespace M_FILE {
	constexpr U64 FILE_A{ 0x8080808080808080ULL };
	constexpr U64 FILE_B{ 0x4040404040404040ULL };
	constexpr U64 FILE_C{ 0x2020202020202020ULL };
	constexpr U64 FILE_D{ 0x1010101010101010ULL };
	constexpr U64 FILE_E{ FILE_D >> 1 };
	constexpr U64 FILE_F{ FILE_E >> 1 };
	constexpr U64 FILE_G{ 0x202020202020202ULL };
	constexpr U64 FILE_H{ 0x101010101010101ULL };

	constexpr U64 FILE_AB{ M_FILE::FILE_A | M_FILE::FILE_B };
	constexpr U64 FILE_GH{ M_FILE::FILE_G | M_FILE::FILE_H };
}

namespace M_RANK {
	constexpr U64 RANK_1{ 0x00000000000000ffULL };
	constexpr U64 RANK_2{ RANK_1 << 8 };
	constexpr U64 RANK_3{ 0x0000000000ff0000ULL };
	constexpr U64 RANK_4{ 0x00000000ff000000ULL };
	constexpr U64 RANK_5{ 0x000000ff00000000ULL };
	constexpr U64 RANK_6{ 0x0000ff0000000000ULL };
	constexpr U64 RANK_7{ 0x00ff000000000000ULL };
	constexpr U64 RANK_8{ 0xff00000000000000ULL };
}


enum Result { BLACK_WIN, WHITE_WIN, DRAW, PLAYING };
enum Pieces{wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK};
enum PurePieces { P, N, B, R, Q, K };
enum Ranks { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };
enum Files { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
enum Colors{WHITE, BLACK};

enum MoveType { quiet, capture, promotion, check, castling, enpasant };



constexpr int PIECE_VALUE[12]{ 100, 300, 330, 500, 900, 10'000'000, -100, -300, -330, -500, -900, -10'000'000 };