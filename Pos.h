#pragma once
#include <iostream>
using U64 = unsigned long long;
using U32 = unsigned int;
class Pos
{
public:
    Pos(const char* p, const char* f, int d, U64 corRes) :
        pos{ p }, fen{ f }, depth{ d }, correctRes{ corRes }
    { }
    const char* pos;
    const char* fen;
    int depth;
    U64 correctRes;
    const char* move[2*218];
    U64 out[218]; // 1 extra space to keep the total
    U64 totalPerfMoves;
    U32 totalAvMoves;
    bool perMove{ false };
    //std::string results;
};

template <int size>
std::ostream& operator<<(std::ostream& out, Pos(&positions)[size])
{
    if (positions->perMove)
    {
        for (int i{ 0 }; i < size; ++i) {
            out << positions[i].pos << ":\n";
            U32 current{ 0 };
            for (U32 j{ 0 }; j < (positions[i].totalAvMoves); ++j)
            {
                out << positions[i].move[current++] << positions[i].move[current++] << ": " << positions[i].out[j] << '\n';
            }
            out << "Total: " << positions[i].totalPerfMoves << '\n';
            out << ((positions[i].totalPerfMoves == positions[i].correctRes) ?
                "correct Result\n" : "XXX__INCORRECT_RESULT__XXX\n");
            out << "++++++++++++++++++++++++++\n\n\n";
        }
        return out;
    }
    for (int i{ 0 }; i < size; ++i)
    {
        out << positions[i].pos << ":\n";
        for (int l{ 0 }; l < positions[i].depth; ++l)
        {
            out << positions[i].out[l] << "\n";
        }
        out << "++++++++++++++++++++++++++\n\n\n";
    }
    return out;
}