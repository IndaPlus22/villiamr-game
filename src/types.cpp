#include "types.hpp"

PieceType operator++(PieceType& pieceType,int){
    pieceType =  static_cast<PieceType>((static_cast<int>(pieceType) + 1) % 13);
    return pieceType;
}