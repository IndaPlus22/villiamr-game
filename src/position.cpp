#include "position.hpp"



/*
    Position constructor intitializes boardstate from a FEN string.
*/
Position::Position(){
    initHashKeys();
}

Position::Position(std::string fen) {
    // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" DEFAULT FEN
    setFen(fen);

}

std::vector<Bitboard> usedKeys = {};

bool isKeyUsed(Bitboard key) {
    for (Bitboard usedKey : usedKeys) {
        if (usedKey == key) {
            return true;
        }
    }
    return false;
}

void Position::initHashKeys(){
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<Bitboard> dis;

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            keys.zobristKeys[i][j] = dis(gen);
            if(isKeyUsed(keys.zobristKeys[i][j])){
                j--;
            }else{
                usedKeys.push_back(keys.zobristKeys[i][j]);
            }
        }
    }

    for (int i = 0; i < 64; i++) {
        keys.zobristEnPassant[i] = dis(gen);
        if(isKeyUsed(keys.zobristEnPassant[i])){
            i--;
        }else{
            usedKeys.push_back(keys.zobristEnPassant[i]); 
        }     
    }

    for (int i = 0; i < 16; i++) {
        keys.zobristCastling[i] = dis(gen);
        if(isKeyUsed(keys.zobristCastling[i])){
            i--;
        }else{
            usedKeys.push_back(keys.zobristCastling[i]);
        }
    }

    do{
        keys.zobristSideToMove = dis(gen);
    } while (isKeyUsed(keys.zobristSideToMove));

    usedKeys.push_back(keys.zobristSideToMove);
}

void Position::printBoard(Bitboard board){
    for (int i = 0; i < 64; i++){
        if (board & (1ULL << i)){
            std::cout << "1 ";
        } else {
            std::cout << "0 ";
        }
        if (i % 8 == 7){
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void Position::setFen(std::string fen){
        this->gameIsOver = false;
    this->checkmate = false;
    this->stalemate = false;

    this->halfMoveCounter = 0;

    // Set all pieces to 0
    for (PieceType i = wPAWN; i < NO_PIECE; i++){
        this->pieces[i] = 0;
    }
    this->allPieces[WHITE] = 0;
    this->allPieces[BLACK] = 0;
    this->occupiedSquares = 0;

    int placementEnd = fen.find(' ');
    std::string placement = fen.substr(0, placementEnd);

    int placementIndex = 0;
    for (char c: placement){
        switch (c){
        case 'p':
            pieces[bPAWN] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'n':
            pieces[bKNIGHT] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'b':
            pieces[bBISHOP] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'r':
            pieces[bROOK] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'q':
            pieces[bQUEEN] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'k':
            pieces[bKING] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'P':   
            pieces[wPAWN] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'N':
            pieces[wKNIGHT] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'B':
            pieces[wBISHOP] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'R':
            pieces[wROOK] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'Q':
            pieces[wQUEEN] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case 'K':
            pieces[wKING] |= (1ULL << placementIndex);
            placementIndex++;
            break;
        case '/':
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            placementIndex += c - '0';
            break;
        
        default:
            std::cout << "Invalid FEN string" << std::endl;
            exit(EXIT_FAILURE);
            break;
        }
    }

    // Set all pieces
    allPieces[WHITE] = (pieces[wPAWN] | pieces[wKNIGHT] | pieces[wBISHOP] | pieces[wROOK] | pieces[wQUEEN] | pieces[wKING]);
    allPieces[BLACK] = (pieces[bPAWN] | pieces[bKNIGHT] | pieces[bBISHOP] | pieces[bROOK] | pieces[bQUEEN] | pieces[bKING]);
    occupiedSquares = (allPieces[WHITE] | allPieces[BLACK]);

    // Set side to move
    if ( fen[placementEnd + 1] == 'w' ){
        sideToMove = WHITE;
    } else if ( fen[placementEnd + 1] == 'b' ){
        sideToMove = BLACK;
    } else {
        std::cout << "Invalid FEN string" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set castling rights
    int castlingRightsBegin = placementEnd + 3;
    int castlingRightsEnd = fen.find(' ', castlingRightsBegin);
    std::string castlingRightsString = fen.substr(castlingRightsBegin, castlingRightsEnd - castlingRightsBegin);
    for (char c: castlingRightsString){
        switch (c){
        case 'K':
            castlingRights |= WHITE_OO;
            break;
        case 'Q':
            castlingRights |= WHITE_OOO;
            break;
        case 'k':
            castlingRights |= BLACK_OO;
            break;
        case 'q':
            castlingRights |= BLACK_OOO;
            break;
        case '-':
            break;
        default:
            std::cout << "Invalid FEN string" << std::endl;
            exit(EXIT_FAILURE);
            break;
        }
    }

    // Set en passant square
    int enPassantSquareBegin = castlingRightsEnd + 1;
    int enPassantSquareEnd = fen.find(' ', enPassantSquareBegin);
    std::string enPassantSquareString = fen.substr(enPassantSquareBegin, enPassantSquareEnd - enPassantSquareBegin);
    if (enPassantSquareString == "-"){
        enPassantSquare = 0;
    } else {
        try {
            enPassantSquare = (1ULL << squareString.at(enPassantSquareString));
        } catch (const std::out_of_range& oor) {
            std::cout << "Invalid FEN string" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Set fiftymovecounter clock
    int halfmoveClockBegin = enPassantSquareEnd + 1;
    int halfmoveClockEnd = fen.find(' ', halfmoveClockBegin);
    std::string halfmoveClockString = fen.substr(halfmoveClockBegin, halfmoveClockEnd - halfmoveClockBegin);
    this->fiftyMoveCounter = std::stoi(halfmoveClockString);

    this->repetitionCounter = 0;

    hash = hashPosition();
}

void Position::printEnPassantBoard(){
    printBoard(enPassantSquare);
}

Bitboard Position::hashPosition(){
    Bitboard hash = 0;

    Bitboard pieceBB;
    for (PieceType p = wPAWN; p < NO_PIECE; p++) {
        pieceBB = pieces[p];
        while (pieceBB) {
            int square = std::countr_zero(pieceBB);
            pieceBB ^= (1ULL << square);
            hash ^= keys.zobristKeys[p][square];
        }
    }

    if (enPassantSquare) {
        hash ^= keys.zobristEnPassant[std::countr_zero(enPassantSquare)];
    }

    hash ^= keys.zobristCastling[castlingRights];

    if (sideToMove == BLACK) {
        hash ^= keys.zobristSideToMove;
    }

    return hash;
}

void Position::makeMove(move currmove){
    PieceType piece = getPieceType(getFromSquare(currmove));
    PieceType capturedPiece = getPieceType(getToSquare(currmove));
    MoveType moveType = getMoveType(currmove);
    Bitboard enPassantMask = enPassantSquare ? enPassantSquare : 0;
    int fromSquare = getFromSquare(currmove);
    int toSquare = getToSquare(currmove);

    halfMoveCounter++;

    // Update State History
    stateHistory.push_back(StateInfo{currmove, capturedPiece, hash, fiftyMoveCounter, repetitionCounter,enPassantSquare, castlingRights});

    pieces[piece] ^= (1ULL << fromSquare | (1ULL << toSquare));  
    
    hash ^= keys.zobristKeys[piece][fromSquare];
    hash ^= keys.zobristKeys[piece][toSquare];

  

    if (moveType == CAPTURE){
        pieces[capturedPiece] &= ~(1ULL << toSquare);

        hash ^= keys.zobristKeys[capturedPiece][toSquare];
    }

    else if (moveType == EN_PASSANT){
        if(sideToMove == WHITE){
            pieces[bPAWN] &= ~(1ULL << (toSquare + 8));

            hash ^= keys.zobristKeys[bPAWN][toSquare + 8];
        } else {
            pieces[wPAWN] &= ~(1ULL << (toSquare - 8));

            hash ^= keys.zobristKeys[wPAWN][toSquare - 8];
        }
    }

    else if (moveType == PROMOTON){
        pieces[piece] &= ~(1ULL << toSquare);

        hash ^= keys.zobristKeys[piece][toSquare];
        if (sideToMove == WHITE){
            pieces[wQUEEN] |= (1ULL << toSquare);

            hash ^= keys.zobristKeys[wQUEEN][toSquare];
        } else {
            pieces[bQUEEN] |= (1ULL << toSquare);

            hash ^= keys.zobristKeys[bQUEEN][toSquare];
        }
    }

    else if (moveType == PROMOTION_CAPTURE){
        pieces[capturedPiece] &= ~(1ULL << toSquare);
        pieces[piece] &= ~(1ULL << toSquare);

        hash ^= keys.zobristKeys[capturedPiece][toSquare];
        hash ^= keys.zobristKeys[piece][toSquare];
        if (sideToMove == WHITE){
            pieces[wQUEEN] |= (1ULL << toSquare);

            hash ^= keys.zobristKeys[wQUEEN][toSquare];
        } else {
            pieces[bQUEEN] |= (1ULL << toSquare);

            hash ^= keys.zobristKeys[bQUEEN][toSquare];
        }
    }

    else if (moveType == CASTLING){
        if (sideToMove == WHITE){
            if (toSquare == G8){
                pieces[wROOK] ^= ((1ULL << H8) | (1ULL << F8));

                hash ^= keys.zobristKeys[wROOK][H8];
                hash ^= keys.zobristKeys[wROOK][F8];
            } else {
                pieces[wROOK] ^= ((1ULL << A8) | (1ULL << D8));

                hash ^= keys.zobristKeys[wROOK][A8];
                hash ^= keys.zobristKeys[wROOK][D8];
            }
        } else {
            if (toSquare == G1){
                pieces[bROOK] ^= ((1ULL << H1) | (1ULL << F1));

                hash ^= keys.zobristKeys[bROOK][H1];
                hash ^= keys.zobristKeys[bROOK][F1];
            } else {
                pieces[bROOK] ^= ((1ULL << A1) | (1ULL << D1));

                hash ^= keys.zobristKeys[bROOK][A1];
                hash ^= keys.zobristKeys[bROOK][D1];
            }
        }
    }

    if(enPassantSquare){
        hash ^= keys.zobristEnPassant[std::countr_zero(enPassantSquare)];
    }

    enPassantSquare = 0;

       // Update en passant square
    if (moveType == DOUBLE_PAWN_PUSH){
        if (piece == wPAWN){
            enPassantSquare = (1ULL << (toSquare + 8));
            hash ^= keys.zobristEnPassant[std::countr_zero(enPassantSquare)];
        } else {
            enPassantSquare = (1ULL << (toSquare - 8));
            hash ^= keys.zobristEnPassant[std::countr_zero(enPassantSquare)];
        }
    } 

    hash ^= keys.zobristCastling[castlingRights];

        // Update castling rights
    if (piece == wKING){
        castlingRights &= ~(WHITE_FULL);
    } else if (piece == bKING){
        castlingRights &= ~(BLACK_FULL);
    } else if (piece == bROOK){
        if (fromSquare == A1){
            castlingRights &= ~BLACK_OOO;
        } else if (fromSquare == H1){
            castlingRights &= ~BLACK_OO;
        }
    } else if (piece == wROOK){
        if (fromSquare == A8){
            castlingRights &= ~WHITE_OOO;
        } else if (fromSquare == H8){
            castlingRights &= ~WHITE_OO;
        }
    }

    hash ^= keys.zobristCastling[castlingRights];

    // Update fifty move counter
    if (piece == wPAWN || piece == bPAWN || capturedPiece != NO_PIECE){
        fiftyMoveCounter = 0;
    } else {
        fiftyMoveCounter++;
    }

    // Update repetition counter
    if(currmove == stateHistory[stateHistory.size() - 5].lastMove){ // If the last move is the same as the move 3 moves ago (i.e. this move minus 2 halfmoves acunting for the current move)
        repetitionCounter++;
    } else {
        repetitionCounter = 0;
    }
    

    // Update side to move
    sideToMove = sideToMove == WHITE ? BLACK : WHITE;
    hash ^= keys.zobristSideToMove;

    hash = hashPosition();

    // if(moveType == CASTLING){
    //     hash = hashPosition();
    // }

    // if (hash != hashPosition()){ // TODO: Remove this but keep for a while
    //     std::cout << "Hash: " << hash << std::endl;
    //     std::cout << "Hash position: " << hashPosition() << std::endl;
    //     std::cout << "Move: " << getFromSquare(currmove) << " " << getToSquare(currmove) << std::endl;
    //     std::cout << "Move type: " << getMoveType(currmove) << std::endl;
    //     printBoard(pieces[piece]);
    //     printBoard(pieces[capturedPiece]);
    //     std::cin >> currmove;
    // }

}

void Position::undoMove(){
    StateInfo state = stateHistory.back();
    stateHistory.pop_back();
    MoveType moveType = getMoveType(state.lastMove);
    PieceType piece = getPieceType(getToSquare(state.lastMove));
    PieceType capturedPiece = state.capturedPiece;
    int fromSquare = getFromSquare(state.lastMove);
    int toSquare = getToSquare(state.lastMove);

    halfMoveCounter--;

    hash = state.hash;

    // Update castling rights
    castlingRights = state.castlingRights;

    // Update en passant square
    enPassantSquare = state.enPassantSquare;

    // Update fifty move counter
    fiftyMoveCounter = state.fiftyMoveCounter;

    // Update repetition counter
    repetitionCounter = state.repetitionCounter;

    // Update side to move
    sideToMove = sideToMove == WHITE ? BLACK : WHITE;

    checkmate = false;
    stalemate = false;

    // game over
    gameIsOver = false;

    if (moveType == QUIET || moveType == DOUBLE_PAWN_PUSH){
        pieces[piece] ^= (1ULL << fromSquare | (1ULL << toSquare));
    }

    else if (moveType == CAPTURE){
        pieces[capturedPiece] |= (1ULL << toSquare);
        pieces[piece] ^= (1ULL << fromSquare | (1ULL << toSquare));
    }

    else if (moveType == EN_PASSANT){
        pieces[piece] ^= (1ULL << fromSquare | (1ULL << toSquare));
        if(sideToMove == WHITE){
            pieces[bPAWN] |= (1ULL << (toSquare + 8));
        } else {
            pieces[wPAWN] |= (1ULL << (toSquare - 8));
        }
    }

    else if (moveType == PROMOTON){
        pieces[sideToMove == WHITE ? wPAWN : bPAWN] |= (1ULL << fromSquare);
        if (sideToMove == WHITE){
            pieces[wQUEEN] &= ~(1ULL << toSquare);
        } else {
            pieces[bQUEEN] &= ~(1ULL << toSquare);
        }
    }

    else if (moveType == PROMOTION_CAPTURE){
        pieces[capturedPiece] |= (1ULL << toSquare);
        pieces[sideToMove == WHITE ? wPAWN : bPAWN] |= (1ULL << fromSquare);
        if (sideToMove == WHITE){
            pieces[wQUEEN] &= ~(1ULL << toSquare);
        } else {
            pieces[bQUEEN] &= ~(1ULL << toSquare);
        }
    }
    
    else if (moveType == CASTLING){
        if (sideToMove == WHITE){
            if (toSquare == G8){
                pieces[wKING] ^= ((1ULL << E8) | (1ULL << G8));
                pieces[wROOK] ^= ((1ULL << H8) | (1ULL << F8));
            } else {
                pieces[wKING] ^= ((1ULL << E8) | (1ULL << C8));
                pieces[wROOK] ^= ((1ULL << A8) | (1ULL << D8));
            }
        } else {
            if (toSquare == G1){
                pieces[bKING] ^= ((1ULL << E1) | (1ULL << G1));
                pieces[bROOK] ^= ((1ULL << H1) | (1ULL << F1));
            } else {
                pieces[bKING] ^= ((1ULL << E1) | (1ULL << C1));
                pieces[bROOK] ^= ((1ULL << A1) | (1ULL << D1));
            }
        }
    }
    
}

void Position::updateAllPiecesBitboard(){
    allPieces[WHITE] = (pieces[wPAWN] | pieces[wKNIGHT] | pieces[wBISHOP] | pieces[wROOK] | pieces[wQUEEN] | pieces[wKING]);
    allPieces[BLACK] = (pieces[bPAWN] | pieces[bKNIGHT] | pieces[bBISHOP] | pieces[bROOK] | pieces[bQUEEN] | pieces[bKING]);
    occupiedSquares = (allPieces[WHITE] | allPieces[BLACK]);
}