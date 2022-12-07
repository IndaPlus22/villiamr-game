#include "position.hpp"

template <PieceColor color>
Bitboard pawnSinglePush (Bitboard pieces, Bitboard occupied) {
    Bitboard moves = 0;
    moves = color == WHITE ? pieces >> 8 : pieces << 8;
    moves &= ~occupied;
    return moves;
}

template <PieceColor color>
Bitboard pawnDubblePush (Bitboard pieces, Bitboard occupied) {
    Bitboard moves = 0;
    pieces &= color == WHITE ? RANK_2 : RANK_7;
    moves = color == WHITE ? pieces >> 16 : pieces << 16;
    moves &= ~occupied;
    return moves;
}

template <PieceColor color>
Bitboard pawnAttacksEast (Bitboard pieces, Bitboard enemy) {
    Bitboard moves = 0;
    pieces &= ~FILE_H;
    moves = color == WHITE ? pieces >> 7 : pieces << 9;
    moves &= enemy;
    return moves;
}

template <PieceColor color>
Bitboard pawnAttacksWest (Bitboard pieces, Bitboard enemy) {
    Bitboard moves = 0;
    pieces &= ~FILE_A;
    moves = color == WHITE ? pieces >> 9 : pieces << 7;
    moves &= enemy;
    return moves;
}



std::vector<Cmove> Position::pawnMoves(){
    std::vector<Cmove> moves;
    Bitboard singlePush;
    Bitboard dubblePush;
    Bitboard attacksEast;
    Bitboard attacksWest;
    int pop;


    // Generate pawn moves
    if ( sideToMove == WHITE){
        singlePush = pawnSinglePush<WHITE>(pieceBitboards[wPAWN], allBitboard);
        dubblePush = pawnDubblePush<WHITE>(pieceBitboards[wPAWN], allBitboard);
        attacksEast = pawnAttacksEast<WHITE>(pieceBitboards[wPAWN], colorBitboards[BLACK]);
        attacksWest = pawnAttacksWest<WHITE>(pieceBitboards[wPAWN], colorBitboards[BLACK]);
    }
    else{
        singlePush = pawnSinglePush<BLACK>(pieceBitboards[bPAWN], allBitboard);
        dubblePush = pawnDubblePush<BLACK>(pieceBitboards[bPAWN], allBitboard);
        attacksEast = pawnAttacksEast<BLACK>(pieceBitboards[bPAWN], colorBitboards[WHITE]);
        attacksWest = pawnAttacksWest<BLACK>(pieceBitboards[bPAWN], colorBitboards[WHITE]);
    }

    return moves;
}

void Position::generateLegalMoves(){
    // Update masks for all pieces and colored pieces
    for (PieceType i = wPAWN; i < bPAWN; i++) {
        colorBitboards[WHITE] |= pieceBitboards[i];
        colorBitboards[BLACK] |= pieceBitboards[i + 6];
    }
    allBitboard = colorBitboards[WHITE] | colorBitboards[BLACK];


    std::vector<Cmove> legalMoves;

    // Generate legal moves for all pieces
    std::vector<Cmove> pawnmoves = pawnMoves();

}


