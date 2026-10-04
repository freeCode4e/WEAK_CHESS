#include <iostream>
#include "types.h"
#include "BitWiseFunctions.h"
#include "Board.h"
#include "Game.h"
#include "../../HEADERS/timing/theChernoTimer.h"
#include <SFML/Graphics.hpp>



#define KNIGHT_MOVES board.N_list, board.totalKnightMoves
#define PAWN_MOVES board.P_list, board.totalPawnMoves
#define ROOK_MOVES board.R_list, board.totalRookMoves


void perftFen(const char* fen, int depth, Pos* pos)
{
    pos->depth = depth;
    Board board;
    board.parseFen(fen);
    for (int i{ 1 }; i <= depth; ++i)
    {
        board.perf(i);
        pos->out[i-1] = board.perfMoves;
        //std::cout << board.perfMoves << "\n\n\n";
        board.perfMoves = 0;
    }
}
//const char* fens[]
//{
//    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
//    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
//    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
//    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
//    "1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1",
//};

int main()
{
    Game game;
    game.play();
    //Board board;
    //Timer timer("Threads timer");
    //board.perfTestsTh();
    ////board.performanceTests();
    //timer.stop();
   /* sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);*/
   // std::cout << arraySize(fens);
    //Pos pos3{"Position 3"};
    //Pos pos4w{"Position 4w"};
    //Pos pos4b{"Position 4b"};
    //Pos pos5{"Position 5"};
    //Pos pos6{"Position 6"};

    //std::thread worker1(function, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, &pos4w);
    //std::thread worker2(function, "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 6, &pos4b);
    //std::thread worker3(function, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, &pos5);
    //std::thread worker4(function, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 6, &pos6);

    ////function("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ", 4, &pos6);
    ////function("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4, &pos5);
    ////function("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 5, &pos4b);
    ////function("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, &pos4w);
    //function("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 8, &pos3);

    //worker4.join();
    //worker3.join();
    //worker2.join();
    //worker1.join();
    //Pos positions[5]{ pos3, pos4w, pos4b, pos5, pos6 };

    //for (int i{ 0 }; i < 5; ++i)
    //{
    //    std::cout << positions[i].pos << ":\n";
    //    for (int l{ 0 }; l < positions[i].depth; ++l)
    //    {
    //        std::cout << positions[i].out[l] << "\n";
    //    }
    //    std::cout << "++++++++++++++++++++++++++\n\n\n";
    //}
    //Game game;
    //game.board.parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1 ");
    //game.board.printBoard();
    //game.play();

    //Board board;
    //board.performanceTests();
    /*Board board;
    
    board.parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.perfPerMove(6);*/
    //std::cout << board.perfMoves;

   // std::cout << board.perfMoves;
    /*for (int i{ 1 }; i < 6; ++i)
    {
        board.perf(i);
        std::cout << board.perfMoves << '\n';
        board.perfMoves = 0;
    }*/
   
    //board.playGame();

    //board._clearBoard_();
    //board.wturn = false;
    //board._add_piece(bK, E2);
    //board._add_piece(wK, D8);
    //board._add_piece(bN, C2);
    //board._add_piece(wR, E7);
    //board.updateBitBoards();
    //board.updatePosHis();
    ////board.printBoard();
    //board.generateAllMoves();
    //board.printBoard();
    //board._go_through_the_moves();
    //board.playGame();
   

   
    return 0;
    // 1111 1111 1111 1111 0000 0000 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 
    // 0000 0010 0000 0010 0000 0010 0000 0010 0000 0010 0000 0010 0000 0010 0000 0010 
}
    /*for (int i{ FILE_A }; i <= FILE_H; ++i)
    {
        board.printBitBoard(M_FILE::FILE_A >> i);
        std::cin.get();
        system("cls");
    }
    for (int i{ RANK_1 }; i <= RANK_8; ++i) {
        board.printBitBoard(M_RANK::RANK_1 << i * 8);
        std::cin.get();
        system("cls");
    }*/
    /*U64 number{ 0 };
    set_bit(number, 10);
    set_bit(number, 4);
    clear_bit(number, 4);
    std::cout << "number: " << number << '\n';
    std::cout << "4th bit: " << get_bit(number, 10) << '\n';
    std::cout << "Index of LSB: " << LSB(number) << '\n';*/


    /*for (int i{ 0 }; i < 64; ++i)
       {
           board._printBitBoard(board.K_Moves[i]);

       }*/
       /*for (int i{ 0 }; i < 64; ++i)
       {
           board.bitBoard[wK] = (1ULL << i);
           board.updateBitBoards();
           std::cout << "Board:\n\n";
           board.printBoard();
           board.generateAllMoves();
           board._go_through_the_moves();
       }*/

       /*U64 file{ M_FILE::FILE_H };
       board._printBitBoard(file);
       file <<= 8;
       board._printBitBoard(file);*/

       /*U64 rank{ M_RANK::RANK_8 };
       board.printBitBoard(rank);
       std::cin.get();
       system("cls");

       rank = ((rank >> 4) & rank);
       board.printBitBoard(rank);
       std::cin.get();
       system("cls");*/

       /*board.printBoard();
       board._clearBoard_();
       std::cin.get();
       system("cls");
       board.printBoard();
       std::cin.get();
       system("cls");
       set_bit(board.bitBoard[wR], E4);
       set_bit(board.bitBoard[bR], E1);
       set_bit(board.bitBoard[wR], A8);
       set_bit(board.bitBoard[bR], E8);
       set_bit(board.bitBoard[bR], B4);

       set_bit(board.bitBoard[wN], C4);
       set_bit(board.bitBoard[bN], B6);
       board.updateBitBoards();
       board.generateKnightMoves();
       board.generateRookMoves();

       board.printBoard();
       std::cin.get();
       system("cls");

       board.printMoves(KNIGHT_MOVES);
       board.printMoves(ROOK_MOVES);*/


       /*for (int i{ 0 }; i < 64; ++i)
       {
           board.printBitBoard(board.R_Moves[i]);
           std::cin.get();
           system("cls");
       }*/
       /*board.generatePawnMoves();
       board.generateKnightMoves();
       board.printMoves(PAWN_MOVES);
       board.printMoves(KNIGHT_MOVES);

       board.wturn = false;
       board.generatePawnMoves();
       board.generateKnightMoves();
       board.printMoves(PAWN_MOVES);
       board.printMoves(KNIGHT_MOVES);*/

       /*for (U32 i{ 0 }; i < board.totalPawnMoves; ++i)
       {
           board.playMove(board.P_list[i]);
           board.printBoard();
           std::cin.get();
           system("cls");
           board.undoMove(board.P_list[i]);
       }*/
       /*for (int i{ 0 }; i < 64; ++i)
       {
           board.printBitBoard(board.R_Moves[i]);
           std::cin.get();
           system("cls");
       }*/