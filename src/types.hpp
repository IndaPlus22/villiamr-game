#ifndef TYPES
#define TYPES

#include <cstdint>

typedef uint64_t Bitboard;

// Move encoding based on https://www.chessprogramming.org/Encoding_Moves#Information_Required
// <0-5> bit for from square
// <6-11> bit for to square
// <12-16> bit for move type
typedef uint16_t Move;



typedef enum PieceType {
    wPAWN, wBISHOP, wKNIGHT, wROOK, wQUEEN, wKING,
    bPAWN, bBISHOP, bKNIGHT, bROOK, bQUEEN, bKING, NO_PIECE
} PieceType;
PieceType operator++(PieceType& pieceType,int); // Overload the ++ operator for PieceType

typedef enum PieceColor {
    WHITE, BLACK
} PieceColor;

typedef enum CastelingRights {
    WHITE_KINGSIDE, WHITE_QUEENSIDE, BLACK_KINGSIDE, BLACK_QUEENSIDE
} CastelingRights;



#endif