#include "zobrist.hpp"

std::vector<Bitboard> usedKeys = {};

bool isKeyUsed(Bitboard key) {
    for (Bitboard usedKey : usedKeys) {
        if (usedKey == key) {
            return true;
        }
    }
    return false;
}

Zobrist::Zobrist() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<Bitboard> dis;

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            zobristKeys[i][j] = dis(gen);
            if(isKeyUsed(zobristKeys[i][j])){
                j--;
            }else{
                usedKeys.push_back(zobristKeys[i][j]);
            }
        }
    }

    for (int i = 0; i < 64; i++) {
        zobristEnPassant[i] = dis(gen);
        if(isKeyUsed(zobristEnPassant[i])){
            i--;
        }else{
            usedKeys.push_back(zobristEnPassant[i]); 
        }     
    }

    for (int i = 0; i < 16; i++) {
        zobristCastling[i] = dis(gen);
        if(isKeyUsed(zobristCastling[i])){
            i--;
        }else{
            usedKeys.push_back(zobristCastling[i]);
        }
    }

    do{
        zobristSideToMove = dis(gen);
    } while (isKeyUsed(zobristSideToMove));

    usedKeys.push_back(zobristSideToMove);
}

Bitboard Zobrist::generateHash(Position position) {
    Bitboard hash = 0;

    Bitboard pieceBB;
    for (PieceType p = wPAWN; p < NO_PIECE; p++) {
        pieceBB = position.getPieceBitboard(p);
        while (pieceBB) {
            int square = std::countr_zero(pieceBB);
            pieceBB ^= (1ULL << square);
            hash ^= zobristKeys[p][square];
        }
    }

    if (position.getEnpassantSquare()) {
        hash ^= zobristEnPassant[std::countr_zero(position.getEnpassantSquare())];
    }

    hash ^= zobristCastling[position.getCastlingRights()];

    if (position.getSideToMove() == BLACK) {
        hash ^= zobristSideToMove;
    }

    return hash;
}