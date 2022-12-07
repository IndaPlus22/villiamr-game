#include "types.hpp"

PieceType operator++(PieceType& pieceType,int){
    pieceType =  static_cast<PieceType>((static_cast<int>(pieceType) + 1) % 13);
    return pieceType;
}

PieceColor operator++(PieceColor& color,int){
    color =  static_cast<PieceColor>((static_cast<int>(color) + 1) % 2);
    return color;
}

PieceColor operator--(PieceColor& color,int){
    color =  static_cast<PieceColor>((static_cast<int>(color) - 1) % 2);
    return color;
}