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
    pieces &= color == WHITE ? RANK_7 : RANK_2;
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



std::vector<Cmove> Position::pawnMoves(){ // TODO: ADD ENPASSANT AND PROMOTION HANDLING
    std::vector<Cmove> moves;
    Bitboard singlePush;
    Bitboard dubblePush;
    Bitboard attacksEast;
    Bitboard attacksWest;

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

    if ( enPassantSquare != -1){ 
        Bitboard enPassant = 1ULL << enPassantSquare; 
        if ( attacksEast & enPassant){
            moves.push_back(Cmove(sideToMove == WHITE ? enPassantSquare + 7 : enPassantSquare - 7, enPassantSquare, EN_PASSANT));
        }
        if(attacksWest & enPassant){
            moves.push_back(Cmove(sideToMove == WHITE ? enPassantSquare + 9 : enPassantSquare - 9, enPassantSquare, EN_PASSANT));
        }
    }

    Bitboard originSQ;
    int from;
    int to;

    // Generate single push moves
    if (sideToMove == WHITE){
        originSQ = pawnSinglePush<BLACK>(singlePush, 0);
    }else {
        originSQ = pawnSinglePush<WHITE>(singlePush, 0);
    }
    while (singlePush){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(singlePush);
        singlePush ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, sideToMove == WHITE ? (to < 8 ? PROMOTION : NORMAL) : (to > 55 ? PROMOTION : NORMAL)));
    }

    // Generate dubble push moves
    if (sideToMove == WHITE){
        originSQ = pawnSinglePush<BLACK>(pawnSinglePush<BLACK>(dubblePush,0), 0); // UGLY ASF but i have no time
    }else {
        originSQ = pawnSinglePush<WHITE>(pawnSinglePush<WHITE>(dubblePush,0), 0);
    }
    while (dubblePush){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(dubblePush);
        dubblePush ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, NORMAL));
    }

    // Generate attacks east moves
    if (sideToMove == WHITE){
        originSQ = pawnAttacksWest<BLACK>(attacksEast, 0);
    }else {
        originSQ = pawnAttacksWest<WHITE>(attacksEast, 0);
    }
    while (attacksEast){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(attacksEast);
        attacksEast ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, sideToMove == WHITE ? (to < 8 ? PROMOTION_CAPTURE : CAPTURE) : (to > 55 ? PROMOTION_CAPTURE : CAPTURE)));
    }

    // Generate attacks west moves
    if (sideToMove == WHITE){
        originSQ = pawnAttacksEast<BLACK>(attacksWest, 0);
    }else {
        originSQ = pawnAttacksEast<WHITE>(attacksWest, 0);
    }
    while (attacksWest){
        from = std::countr_zero(originSQ);
        to = std::countr_zero(attacksWest);
        attacksWest ^= 1ULL << to;
        originSQ ^= 1ULL << from;
        moves.push_back(Cmove(from, to, sideToMove == WHITE ? (to < 8 ? PROMOTION_CAPTURE : CAPTURE) : (to > 55 ? PROMOTION_CAPTURE : CAPTURE)));
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

    legalMoves.clear();

    // Generate legal moves for all pieces
    std::vector<Cmove> pawnmoves = pawnMoves();
    legalMoves.insert(legalMoves.end(), pawnmoves.begin(), pawnmoves.end());
}


