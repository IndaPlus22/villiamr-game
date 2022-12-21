#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <map>
#include <string>

// Type definitions
typedef uint64_t Bitboard;
typedef uint16_t move;


// typedef enumerations
typedef enum Color { WHITE, BLACK } Color;

typedef enum PieceType { 
    wPAWN, wKNIGHT, wBISHOP, wROOK, wQUEEN, wKING,
    bPAWN, bKNIGHT, bBISHOP, bROOK, bQUEEN, bKING, 

    NO_PIECE
}PieceType;

constexpr PieceType operator++(PieceType& piece, int){
    piece = static_cast<PieceType>((static_cast<int>(piece) + 1) % 13);
    return piece;
}

typedef enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE
}Square;


typedef enum CastlingRights { 
    WHITE_OO = 1,
    WHITE_OOO = 1 << 1,
    BLACK_OO = 1 << 2,
    BLACK_OOO = 1 << 3,

    WHITE_FULL = WHITE_OO | WHITE_OOO,
    BLACK_FULL = BLACK_OO | BLACK_OOO,
    FULL = WHITE_FULL | BLACK_FULL,

    NO_CASTLING = 0
}CastlingRights;

typedef enum MoveType {
    QUIET,
    DOUBLE_PAWN_PUSH,
    CASTLING,
    CAPTURE,
    EN_PASSANT,
    PROMOTON,
    PROMOTION_CAPTURE
}MoveType;


// Constants
const Bitboard FILE_A = 0x0101010101010101;
const Bitboard FILE_B = 0x0202020202020202;
const Bitboard FILE_C = 0x0404040404040404;
const Bitboard FILE_D = 0x0808080808080808;
const Bitboard FILE_E = 0x1010101010101010;
const Bitboard FILE_F = 0x2020202020202020;
const Bitboard FILE_G = 0x4040404040404040;
const Bitboard FILE_H = 0x8080808080808080;
 
const Bitboard RANK_1 = 0x00000000000000FF;
const Bitboard RANK_2 = 0x000000000000FF00;
const Bitboard RANK_3 = 0x0000000000FF0000;
const Bitboard RANK_4 = 0x00000000FF000000;
const Bitboard RANK_5 = 0x000000FF00000000;
const Bitboard RANK_6 = 0x0000FF0000000000;
const Bitboard RANK_7 = 0x00FF000000000000;
const Bitboard RANK_8 = 0xFF00000000000000;

const Bitboard FILES[] = {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

const Bitboard RANKS[] = {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

// Constexpr functions

constexpr move encodeMove(Square from, Square to, MoveType moveType) {
    return from | (to << 6) | (moveType << 12);
}

constexpr move encodeMove(int from,int to,MoveType moveType) {
    return from | (to << 6) | (moveType << 12);
}

constexpr int getFromSquare(move m) {
    return (m & 0x3F);
}

constexpr int getToSquare(move m) {
    return ((m >> 6) & 0x3F);
}

constexpr MoveType getMoveType(move m) {
    return static_cast<MoveType>((m >> 12) & 0xF);
}

// Maps 
const std::map<std::string, Square> squareString{
    {"a1", A1}, {"b1", B1}, {"c1", C1}, {"d1", D1}, {"e1", E1}, {"f1", F1}, {"g1", G1}, {"h1", H1},
    {"a2", A2}, {"b2", B2}, {"c2", C2}, {"d2", D2}, {"e2", E2}, {"f2", F2}, {"g2", G2}, {"h2", H2},
    {"a3", A3}, {"b3", B3}, {"c3", C3}, {"d3", D3}, {"e3", E3}, {"f3", F3}, {"g3", G3}, {"h3", H3},
    {"a4", A4}, {"b4", B4}, {"c4", C4}, {"d4", D4}, {"e4", E4}, {"f4", F4}, {"g4", G4}, {"h4", H4},
    {"a5", A5}, {"b5", B5}, {"c5", C5}, {"d5", D5}, {"e5", E5}, {"f5", F5}, {"g5", G5}, {"h5", H5},
    {"a6", A6}, {"b6", B6}, {"c6", C6}, {"d6", D6}, {"e6", E6}, {"f6", F6}, {"g6", G6}, {"h6", H6},
    {"a7", A7}, {"b7", B7}, {"c7", C7}, {"d7", D7}, {"e7", E7}, {"f7", F7}, {"g7", G7}, {"h7", H7},
    {"a8", A8}, {"b8", B8}, {"c8", C8}, {"d8", D8}, {"e8", E8}, {"f8", F8}, {"g8", G8}, {"h8", H8}
};

#endif