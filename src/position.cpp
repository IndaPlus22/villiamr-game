#include "position.hpp"


Position::Position(std::string fen) {
    this->initPosition(fen);
    this->allBitboard = 0;
    this->colorBitboards[WHITE] = 0;
    this->colorBitboards[BLACK] = 0; 
    for (PieceType i = wPAWN; i < bPAWN; i++) {
        this->colorBitboards[WHITE] |= this->pieceBitboards[i];
        this->colorBitboards[BLACK] |= this->pieceBitboards[i + 6];
        this->allBitboard |= this->pieceBitboards[i];
        this->allBitboard |= this->pieceBitboards[i + 6];
    }

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
    stringIndex++;

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
void Position::makeMove(Move move) {
    PieceType piecetype = getPiceceAtSquare(move & 0x3F); 
    if ((move & 0xF000) == 0){ // Normal move (only moves one pieces from a square to another)
        pieceBitboards[piecetype] ^= (1ULL << (move & 0x3F));
        pieceBitboards[piecetype] |= (1ULL << ((move >> 6) & 0x3F));
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