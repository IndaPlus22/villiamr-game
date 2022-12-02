#ifndef TYPES
#define TYPES

typedef enum PieceType {
    wPAWN, wBISHOP, wKNIGHT, wROOK, wQUEEN, wKING,
    bPAWN, bBISHOP, bKNIGHT, bROOK, bQUEEN, bKING, NO_PIECE
} PieceType;
PieceType operator++(PieceType& pieceType,int); // Overload the ++ operator for PieceType

#endif