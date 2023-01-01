#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include "types.hpp"
#include "position.hpp"
#include <random>

class Zobrist {
    private:
        Bitboard zobristKeys[12][64];
        Bitboard zobristEnPassant[64];
        Bitboard zobristCastling[16];
        Bitboard zobristSideToMove;
    public:
        Zobrist();
        ~Zobrist() = default;

};

#endif