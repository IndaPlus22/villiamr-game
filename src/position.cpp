#include "position.hpp"


Position::Position(std::string fen) {
    this->initPosition(fen);
    this->allBitboard = 0;
    this->colorBitboards[WHITE] = 0;
    this->colorBitboards[BLACK] = 0; 
    this->enPassantSquare = -1;
    this->attackboard = 0;
    for (PieceType i = wPAWN; i < bPAWN; i++) {
        this->colorBitboards[WHITE] |= this->pieceBitboards[i];
        this->colorBitboards[BLACK] |= this->pieceBitboards[i + 6];
        this->allBitboard |= this->pieceBitboards[i];
        this->allBitboard |= this->pieceBitboards[i + 6];
    }
    initSlidersAttacks(true); // INTIalize bishop attacks tables
    initSlidersAttacks(false);  // Rook
}

void Position::initPosition(std::string fen) {
    // Handels the piece positions, side to move and castling rights but not en passant square and the other stuff cuz that would suck and I don't care

    int stringIndex = 0;
    int square = 0;
    bool positionInit = true;

    for (PieceType i = wPAWN; i < NO_PIECE; i++) {
        this->pieceBitboards[i] = 0;
    }


    for (char c: fen) {
        if (!positionInit) 
            break;
        stringIndex++;

        switch (c){
        case 'r':                                                
            pieceBitboards[bROOK] |= (1ULL << square);           
            square++;                                            
            break;                                               
        case 'n':
            pieceBitboards[bKNIGHT] |= (1ULL << square);
            square++;
            break;
        case 'b':
            pieceBitboards[bBISHOP] |= (1ULL << square);
            square++;
            break;
        case 'q':
            pieceBitboards[bQUEEN] |= (1ULL << square);
            square++;
            break;
        case 'k':
            pieceBitboards[bKING] |= (1ULL << square);
            square++;
            break;
        case 'p':
            pieceBitboards[bPAWN] |= (1ULL << square);
            square++;
            break;

        case 'R':
            pieceBitboards[wROOK] |= (1ULL << square);
            square++;
            break;
        case 'N':
            pieceBitboards[wKNIGHT] |= (1ULL << square);
            square++;
            break;
        case 'B':
            pieceBitboards[wBISHOP] |= (1ULL << square);
            square++;
            break;
        case 'Q':
            pieceBitboards[wQUEEN] |= (1ULL << square);
            square++;
            break;
        case 'K':  
            pieceBitboards[wKING] |= (1ULL << square);
            square++;
            break;
        case 'P':
            pieceBitboards[wPAWN] |= (1ULL << square);
            square++;
            break;

        case '/':
            break;
        case ' ':
            positionInit = false;
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            square += c - '0';
            break;


        default:
            break;
        }
    }

    if (fen[stringIndex] == 'w') {
        sideToMove = WHITE;
    } else {
        sideToMove = BLACK;
    }
    stringIndex += 2;

    castlingRights = 0;
    for (char c: fen.substr(stringIndex, 4)) {
        switch (c) {
        case 'K':
            castlingRights |= (0x01 << WHITE_KINGSIDE);
            break;
        case 'Q':
            castlingRights |= (0x01 << WHITE_QUEENSIDE);
            break;
        case 'k':
            castlingRights |= (0x01 << BLACK_KINGSIDE);
            break;
        case 'q':
            castlingRights |= (0x01 << BLACK_QUEENSIDE);
            break;
        default:
            break;
        }
    }

}


void Position::printBitboard(PieceType board){
    for (int i = 0; i < 64; i++){
        if (i % 8 == 0)
            std::cout << std::endl;
        if (pieceBitboards[board] & (1ULL << i))
            std::cout << "1";
        else
            std::cout << "0";
    }
    std::cout << std::endl;
}

void Position::printBitboard (){
    for (int i = 0; i < 64; i++){
        if (i % 8 == 0)
            std::cout << std::endl;
        if (allBitboard & (1ULL << i))
            std::cout << "1";
        else
            std::cout << "0";
    }
    std::cout << std::endl;
}

void Position::printBitboard(Bitboard board){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            std::cout << ((board >> (i*8 + j)) & 1);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Position::printBitboard(PieceColor color){
    for (int i = 0; i < 64; i++){
        if (i % 8 == 0)
            std::cout << std::endl;
        if (colorBitboards[color] & (1ULL << i))
            std::cout << "1";
        else
            std::cout << "0";
    }
    std::cout << std::endl;
}

// DOES NOT HANDLE MOVE VALIDITY
void Position::makeMove(Cmove move) { // TODO: UPDATE CASTLING RIGHTS FOR ROOK MOVES
    MoveType type = move.getType();
    PieceType captured = type == CAPTURE || type == PROMOTION_CAPTURE ? getPiceceAtSquare(move.getTo()) : NO_PIECE;
    PieceType piece = getPiceceAtSquare(move.getFrom());
    move.setCapturedPiece(captured);

     // Update castling rights
    if (piece == wROOK && castlingRights & (0x01 << WHITE_QUEENSIDE) && move.getFrom() == 56) {     
        castlingRights &= ~(0x01 << WHITE_QUEENSIDE); 
        move.setRemovedCastelingRights(WHITE_QUEENSIDE);
    }
    else if (piece == wROOK && castlingRights & (0x01 << WHITE_KINGSIDE) && move.getFrom() == 63){
        castlingRights &= ~(0x01 << WHITE_KINGSIDE);
        move.setRemovedCastelingRights(WHITE_KINGSIDE);
    }
    else if (piece == bROOK && castlingRights & (0x01 << BLACK_QUEENSIDE) && move.getFrom() == 0){
        castlingRights &= ~(0x01 << BLACK_QUEENSIDE);
        move.setRemovedCastelingRights(BLACK_QUEENSIDE);
    }
    else if (piece == bROOK && castlingRights & (0x01 << BLACK_KINGSIDE) && move.getFrom() == 7){
        castlingRights &= ~(0x01 << BLACK_KINGSIDE);
        move.setRemovedCastelingRights(BLACK_KINGSIDE);
    }

    // En_Passant square handeling
    if (piece == wPAWN || piece == bPAWN){
        if (piece == wPAWN && (RANK_7 & (1ULL << move.getFrom())) != 0 && (RANK_5 & (1ULL << move.getTo())) != 0){
            enPassantSquare = move.getTo() + 8;
        }
        else if (piece == bPAWN && (RANK_2 & (1ULL << move.getFrom())) != 0 && (RANK_4 & (1ULL << move.getTo())) != 0){
            enPassantSquare = move.getTo() - 8;
        }
        else{
            enPassantSquare = -1;
        }
    }else{
        enPassantSquare = -1;
    }
   

    switch (type) {
    case NORMAL:
        pieceBitboards[piece] ^= (1ULL << move.getFrom());
        pieceBitboards[piece] |= (1ULL << move.getTo());
        break;
    case CAPTURE:
        pieceBitboards[piece] ^= (1ULL << move.getFrom());
        pieceBitboards[piece] |= (1ULL << move.getTo());
        pieceBitboards[captured] ^= (1ULL << move.getTo());
        break;
    case CASTELING:
        if (sideToMove == BLACK){
            if (move.getTo() == 6){
                pieceBitboards[bKING] |= (1ULL << move.getTo());
                pieceBitboards[bKING] ^= (1ULL << move.getFrom());
                pieceBitboards[bROOK] |= (1ULL << 5);
                pieceBitboards[bROOK] ^= (1ULL << 7);
            } else {
                pieceBitboards[bKING] |= (1ULL << move.getTo());
                pieceBitboards[bKING] ^= (1ULL << move.getFrom());
                pieceBitboards[bROOK] |= (1ULL << 3);
                pieceBitboards[bROOK] ^= (1ULL << 0);
            }
            castlingRights &= ~(0x01 << BLACK_QUEENSIDE);
            castlingRights &= ~(0x01 << BLACK_KINGSIDE);
            move.setRemovedCastelingRights(BLACK_QUEENSIDE);    // All in the name of inversability
            move.setRemovedCastelingRights(BLACK_KINGSIDE);
        } else {
            if (move.getTo() == 62){
                pieceBitboards[wKING] |= (1ULL << move.getTo());
                pieceBitboards[wKING] ^= (1ULL << move.getFrom());
                pieceBitboards[wROOK] |= (1ULL << 61);
                pieceBitboards[wROOK] ^= (1ULL << 63);
            } else {
                pieceBitboards[wKING] |= (1ULL << move.getTo());
                pieceBitboards[wKING] ^= (1ULL << move.getFrom());
                pieceBitboards[wROOK] |= (1ULL << 59);
                pieceBitboards[wROOK] ^= (1ULL << 56);
            }
            castlingRights &= ~(0x01 << WHITE_QUEENSIDE);
            castlingRights &= ~(0x01 << WHITE_KINGSIDE);
            move.setRemovedCastelingRights(WHITE_QUEENSIDE);
            move.setRemovedCastelingRights(WHITE_KINGSIDE);
        }
        break;
    case EN_PASSANT:
        pieceBitboards[piece] ^= (1ULL << move.getFrom());
        pieceBitboards[piece] |= (1ULL << move.getTo());
        pieceBitboards[sideToMove == WHITE ? bPAWN : wPAWN] ^= (1ULL << (sideToMove == WHITE ? move.getTo() + 8 : move.getTo() - 8));
        break;
    case PROMOTION:
        pieceBitboards[piece] ^= (1ULL << move.getFrom());
        pieceBitboards[sideToMove == WHITE ? wQUEEN : bQUEEN] |= (1ULL << move.getTo());
        break;
    case PROMOTION_CAPTURE:
        pieceBitboards[piece] ^= (1ULL << move.getFrom());
        pieceBitboards[captured] ^= (1ULL << move.getTo());
        pieceBitboards[sideToMove == WHITE ? wQUEEN : bQUEEN] |= (1ULL << move.getTo());
        break;
    }
    sideToMove++;
    moveLog.push_back(move);
}

 void Position::unmakeMove(){ // TODO: UPDATE CASTLING RIGHTS
    Cmove move = moveLog.back();
    PieceType piece = getPiceceAtSquare(move.getTo());
    PieceType captured = move.getCapturedPiece();
    Move fromSQ = move.getFrom();
    moveLog.pop_back();
    sideToMove++; // SWITches back move turn

    // Gives back castling rights
    if (move.getRemovedCastelingRights() != 0){             
        castlingRights |= move.getRemovedCastelingRights();
    }

    // En_Passant square handeling TODO: FIX



    switch (move.getType()) {
    case NORMAL: // When no pieces are removed we reverse make move function
        move.setFrom(move.getTo());
        move.setTo(fromSQ);
        makeMove(move);
        break;
    case CASTELING:
        if (sideToMove == BLACK){
            if (move.getTo() == 6){
                pieceBitboards[bKING] ^= (1ULL << move.getTo());
                pieceBitboards[bKING] |= (1ULL << move.getFrom());
                pieceBitboards[bROOK] ^= (1ULL << 5);
                pieceBitboards[bROOK] |= (1ULL << 7);
            } else {
                pieceBitboards[bKING] ^= (1ULL << move.getTo());
                pieceBitboards[bKING] |= (1ULL << move.getFrom());
                pieceBitboards[bROOK] ^= (1ULL << 3);
                pieceBitboards[bROOK] |= (1ULL << 0);
            }
        } else {
            if (move.getTo() == 62){
                pieceBitboards[wKING] ^= (1ULL << move.getTo());
                pieceBitboards[wKING] |= (1ULL << move.getFrom());
                pieceBitboards[wROOK] ^= (1ULL << 61);
                pieceBitboards[wROOK] |= (1ULL << 63);
            } else {
                pieceBitboards[wKING] ^= (1ULL << move.getTo());
                pieceBitboards[wKING] |= (1ULL << move.getFrom());
                pieceBitboards[wROOK] ^= (1ULL << 59);
                pieceBitboards[wROOK] |= (1ULL << 56);
            }
        }
        break;
    case CAPTURE:
        pieceBitboards[piece] ^= (1ULL << move.getTo());
        pieceBitboards[piece] |= (1ULL << move.getFrom());
        pieceBitboards[captured] |= (1ULL << move.getTo());
        break;
    case EN_PASSANT:
        pieceBitboards[piece] ^= (1ULL << move.getTo());
        pieceBitboards[piece] |= (1ULL << move.getFrom());
        pieceBitboards[sideToMove == WHITE ? bPAWN : wPAWN] |= (1ULL << (sideToMove == WHITE ? move.getTo() + 8 : move.getTo() - 8));
        break;
    case PROMOTION:
        pieceBitboards[sideToMove == WHITE ? wPAWN : bPAWN] |= (1ULL << move.getFrom());
        pieceBitboards[sideToMove == WHITE ? wQUEEN : bQUEEN] ^= (1ULL << move.getTo());
        break;
    case PROMOTION_CAPTURE:
        pieceBitboards[sideToMove == WHITE ? wPAWN : bPAWN] |= (1ULL << move.getFrom());
        pieceBitboards[captured] |= (1ULL << move.getTo());
        pieceBitboards[sideToMove == WHITE ? wQUEEN : bQUEEN] ^= (1ULL << move.getTo());
        break;
    }
 }

PieceType Position::getPiceceAtSquare(int square) {
    for (PieceType i = wPAWN; i < NO_PIECE; i++) {
        if (pieceBitboards[i] & (1ULL << square)) {
            return i;
        }
    }
    return NO_PIECE;
}

