#ifndef POSITION_HPP
#define POSITION_HPP

#include <vector>
#include <iostream>
#include "types.hpp"


/*
    *  The stateInfo struct is used to store information about the board, needed to undo moves.
*/
typedef struct stateInfo {
    move lastMove;
    PieceType capturedPiece;

    int fiftyMoveCounter;
    int repetitionCounter;
    Bitboard enPassantSquare;
    uint8_t castlingRights;

    bool operator==(const stateInfo& other) const {
        return lastMove == other.lastMove;
    }
}StateInfo;

class Position {
    private:
        Bitboard pieces[12];
        Bitboard allPieces[2];
        Bitboard occupiedSquares;

        Color sideToMove;
        uint8_t castlingRights;
        Bitboard enPassantSquare;

        int fiftyMoveCounter;
        int repetitionCounter;

        int halfMoveCounter;

        std::vector<StateInfo> stateHistory;

        bool gameIsOver;
        bool checkmate;
        bool stalemate;
public:
        Position() = default;
        Position(std::string fen);
        ~Position() = default;

        void makeMove(move m);
        void undoMove();


        Bitboard getPieceBitboard (PieceType piece) const { return this->pieces[piece]; };
        Bitboard getAllPiecesBitboard (Color color) const { return this->allPieces[color]; };
        Bitboard getOccupiedSquaresBitboard () const { return this->occupiedSquares; };

        Bitboard getEnpassantSquare () const { return this->enPassantSquare; };
        uint8_t getCastlingRights () const { return castlingRights; };

        Color getSideToMove () const { return sideToMove; };

        bool getGameIsOver () const { return gameIsOver; };
        bool getCheckmate () const { return checkmate; };
        bool getStalemate () const { return stalemate; };
        int getHalfMoveCounter () const { return halfMoveCounter; };
        int getRepetitionCounter () const { return repetitionCounter; };
        

        void setCheckmate (bool checkmate) { this->checkmate = checkmate; };
        void setStalemate (bool stalemate) { this->stalemate = stalemate; };
        void resetHalfMove() {this->halfMoveCounter = 0;}

        void updateAllPiecesBitboard();

        PieceType getPieceType (Square square) const { // This function being constexpr speeds upp program at runtime but kills compile which we don't care about
            for (PieceType i = wPAWN; i < NO_PIECE; i++){
                if (pieces[i] & (1ULL << square)){
                    return i;
                }
            }
            return NO_PIECE;
        };

        PieceType getPieceType (int square) const { // This function being constexpr speeds upp program at runtime but kills compile which we don't care about
            for (PieceType i = wPAWN; i < NO_PIECE; i++){
                if (pieces[i] & (1ULL << square)){
                    return i;
                }
            }
            return NO_PIECE;
        };


        void printBoard(Bitboard board);
        void printEnPassantBoard();
};


#endif