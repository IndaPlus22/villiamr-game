#ifndef POSITION
#define POSITION

#include <string>
#include <vector>
#include <iostream>
#include <bit>
#include <thread>
#include <mutex>
#include "types.hpp"

class Position {
    public:
        Position(std::string fen);
        //~Position();

        // GETTERS AND SETTERS
        Bitboard getBitboard(PieceType piecetype) {return pieceBitboards[piecetype];};
        std::vector<Cmove> getLegalMoves() {return legalMoves;};
        PieceColor getSideToMove() {return sideToMove;};
        Bitboard getAllBitboard() {return allBitboard;};
        Bitboard getColorBitboard(PieceColor color) {return colorBitboards[color];};
        Bitboard getPieceBitboard(PieceType piecetype) {return pieceBitboards[piecetype];};
        int getEnPassantSquare() {return enPassantSquare;};
        uint8_t getCastelingRights() {return castlingRights;};

        // TESTING FUNCTIONS
        void printBitboard(PieceType board); 
        void printBitboard(PieceColor color);
        void printBitboard(Bitboard board);
        void printBitboard();


        // MOVE FUNCTIONS
        void generateLegalMoves();
        void makeMove(Cmove move);
        void unmakeMove();


    private:
        Bitboard pieceBitboards[12];
        Bitboard colorBitboards[2];
        Bitboard allBitboard;
        Bitboard attackboard; 
        Bitboard castleattackboard;
        Bitboard pins;
        Bitboard checks;

        PieceColor sideToMove;
        uint8_t castlingRights;
        int enPassantSquare;

        std::vector<Cmove> legalMoves;
        std::vector<Cmove> moveLog;

        void getPinsAndChecks();
        void getAttackboard();

        void pawnMoves();
        void knightMoves();
        void bishopMoves();
        void rookMoves();
        void queenMoves();
        void kingMoves();

        void initPosition(std::string fen);
        void initSlidersAttacks(bool is_bishop);

        PieceType getPiceceAtSquare(int square);
        
};



#endif