#ifndef POSITION
#define POSITION

#include <string>
#include <vector>
#include <iostream>
#include "types.hpp"

class Position {
    public:
        Position(std::string fen);
        //~Position();
        
        // GETTERS AND SETTERS
        Bitboard getBitboard(PieceType piecetype) {return pieceBitboards[piecetype];};

        // TESTING FUNCTIONS
        void printBitboard(PieceType board); 
        void printBitboard(PieceColor color);
        void printBitboard();
        uint8_t getCastelingRights() {return castlingRights;};
        PieceColor getSideToMove() {return sideToMove;};



    private:
        Bitboard pieceBitboards[12];
        Bitboard colorBitboards[2];
        Bitboard allBitboard; 

        PieceColor sideToMove;
        uint8_t castlingRights;
        int enPassantSquare;

        std::vector<Move> moveLog;

        void initPosition(std::string fen);
        
};



#endif