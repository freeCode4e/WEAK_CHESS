#pragma once
#include "Board.h"
#include "types.h"
#include "RectProperties_s.h"
#include "Position_s.h"
#include "defs.h"
#include "Color_s.h"
#include "operatorDef.h"
#include <SFML/Graphics.hpp>
#include <iostream>

class Game
{
public:
	U32 SELECTED_DEPTH = 4;
public:
	Game();
	void createBoard(RectProperties_s& props, const Position_s start, U32 rowEls, sf::Color altColor);
	void play();
	void reverseBoard();
	void drawBoard();
	void loadTextures();
	void updateBoard();
	void initSquareColors();
	void initPieceTextures();
	ChessMove getMove();
	bool clicked(const sf::RectangleShape& shape, const sf::Vector2i& mousePos);
	void playMove(ChessMove move);
	void clickAfterGameOver();
	void clickPlayAgain(sf::Vector2i);
	void clickUndo(sf::Vector2i);
	void clickRedo(sf::Vector2i);

public:
	static constexpr U32 BOARD_WIDTH{ 890 };
	static constexpr U32 BOARD_HEIGHT{ 970 };
	static constexpr U32 SQUARES_NUM{ 64 };
	static constexpr U32 SQUARE_SIZE{ 110 };
	static constexpr Position_s START_POS{ 5, 45};
	static constexpr U32 ROW_ELS{ 8 };
	static constexpr U32 PIECES{ 12 };
	static constexpr U32 COLORS{ 2 };
	static constexpr const char* W_SQ_PIECE_TEXTURES[PIECES]
	{
		"whitePawn.png",
		"whiteKnight.png",
		"whiteBishop.png",
		"whiteRook.png",
		"whiteQueen.png",
		"whiteKing.png",

		"blackPawn.png",
		"blackKnight.png",
		"blackBishop.png",
		"blackRook.png",
		"blackQueen.png",
		"blackKing.png"
	};

	static constexpr const char* B_SQ_PIECE_TEXTURES[PIECES]
	{
		"b_whitePawn.png",
		"b_whiteKnight.png",
		"b_whiteBishop.png",
		"b_whiteRook.png",
		"b_whiteQueen.png",
		"b_whiteKing.png",

		"b_blackPawn.png",
		"b_blackKnight.png",
		"b_blackBishop.png",
		"b_blackRook.png",
		"b_blackQueen.png",
		"b_blackKing.png"
	};

	static const char* PIECE_TEXTURES[COLORS][PIECES];
	static U32 squareColors[SQUARES_NUM];
	static constexpr Color_s wSqCol{ 244,202,144 };
	static constexpr Color_s bSqCol{ 169, 101, 35 };
	static constexpr Color_s SQUARE_COLOR[2]{ wSqCol, bSqCol };

	U32 undoneMoves{ 0 };
public:
	Board board;
	sf::RenderWindow window{ sf::VideoMode(BOARD_WIDTH, BOARD_HEIGHT), "Chess!", sf::Style::Close };
	sf::RectangleShape rects[SQUARES_NUM];
	sf::Vector2i windowPos;
	sf::Texture pieces[COLORS][PIECES];
	sf::RectangleShape* selected{ nullptr };
	sf::RectangleShape forceMove;
	sf::Font forceMoveFont;
	sf::Text forceMoveText;
	sf::RectangleShape playToEnd;
	sf::Text playToEndText;
	sf::RectangleShape playAgain;
	sf::Text playAgainText;
	sf::RectangleShape flipBoard;
	sf::Text flipBoardText;

	sf::RectangleShape sideToMove;

	sf::RectangleShape increasePly;
	sf::RectangleShape decreasePly;
	sf::RectangleShape searchPlies;
	sf::Text searchPliesText;
	sf::Text plusText;
	sf::Text minusText;

	sf::RectangleShape computerThinking;
	sf::Texture computerThinkingTexture;

	sf::RectangleShape undo;
	sf::Text undoText;

	sf::RectangleShape prev;
	sf::RectangleShape next;
	sf::Texture prevTexture;
	sf::Texture nextTexture;
	

	ChessMove previous{ 1000,1000 };
	bool playerTurn{ true };
	bool onlyComputerPlays{ false };
	Result res{ PLAYING };
};

