#include "Engine.hpp"

Engine::Engine(int depth){
    this->maxDepth = depth;
    this->bestMove = 0;
    this->bestscore = 0;

    this->stop = false;

}

//*************************************
// MOVE SORTING
//*************************************


// most valuable victim, least valuable attacker table for scoring captures
static const int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

inline int scoreMove(move m,Position pos){
    MoveType type = getMoveType(m);

    if (type == CAPTURE){
        return mvv_lva[pos.getPieceType(getFromSquare(m))][pos.getPieceType(getToSquare(m))];
    }
    if (type == EN_PASSANT){
        return mvv_lva[wPAWN][wPAWN];
    }
    return 0;
}

int partition(std::vector<move> &movelist,int low, int high,Position obj){
    int pivotIDX = low + std::rand() % (high - low);

    int pivVAL = scoreMove(movelist[pivotIDX],obj);
    std::swap(movelist[pivotIDX], movelist[high]);

    int storeIDX = low;
     for (int i = low; i < high; i++) { // loop over array from low to high
        if (scoreMove(movelist[i],obj) > pivVAL) {   // if element is smaller than pivot we sould swap it with the element at storeIndex which is the first element bigger than pivot
            std::swap(movelist[storeIDX], movelist[i]);
            storeIDX++;
        }
    }
    std::swap(movelist[high],movelist[storeIDX]);

    return storeIDX;
}

void qSort (std::vector<move> &movelist,int low, int high, Position obj){
    if (movelist.size() < 5){
        int i = 1;
        while(i < movelist.size()) {
            int x = scoreMove(movelist[i],obj);
            int j = i - 1;
            while (j >= 0 && scoreMove(movelist[j],obj) < x) {
                movelist[j+1] = movelist[j];
                j--;
            }
            movelist[j+1] = x;
            i++;
        }
    }
    if(low < high){
        int pivot = partition(movelist,low,high,obj);
        qSort(movelist,low,pivot,obj);
        qSort(movelist, pivot+1,high,obj);
    }
}

inline void sortMoves(std::vector<move> &movelist, Position obj){
    std::sort(movelist.begin(), movelist.end(),[obj](move a, move b){
        return scoreMove(a,obj) > scoreMove(b,obj);
    });

}


void Engine::findBestMove(Position pos, std::chrono::duration<double> &executionTime){
    nodes = 0;
    bestMove = generateLegalMoves(pos)[0];
    auto start = std::chrono::high_resolution_clock::now();
    minimax(pos, maxDepth, -INT16_MAX, INT16_MAX);
    auto end = std::chrono::high_resolution_clock::now();
    executionTime = end - start;
}


/*  
    MINIMAX algorithm with alpha-beta pruning to find the best move
*/
int Engine::minimax(Position pos,int depth, int alpha, int beta){
    if(stop) return 0;
    if( depth == 0 ) return quiesce(pos,alpha,beta);

    // SORTING MOVES
    std::vector<move> moves = generateLegalMoves(pos);
    sortMoves(moves,pos);
    for ( move m : moves ) {
        pos.makeMove(m);
        nodes++;
        int score = -minimax(pos,depth-1, -beta, -alpha);
        pos.undoMove();
        if( score >= beta )
            return beta;   //  fail hard beta-cutoff
        if( score > alpha ){
            alpha = score; // alpha acts like max in MiniMax

            
            if (depth == maxDepth){
                bestscore = score;
                bestMove = m;
            }
        }
    }
    if(pos.getCheckmate()){ // NEED TO ADD PLY?
        return INT16_MIN + pos.getHalfMoveCounter();
    }if(pos.getStalemate()){
        return 0;
    }
    return alpha;
}

int Engine::quiesce(Position pos, int alpha, int beta){
    int stand_pat = evaluatePosition(pos);
    if( stand_pat >= beta )
        return beta;
    if( alpha < stand_pat )
        alpha = stand_pat;
    std::vector<move> moves = generateLegalMoves(pos);
    sortMoves(moves,pos);
    for ( move m : moves ) {
        if ((getMoveType(m) == CAPTURE)){
            pos.makeMove(m);
            nodes++;
            int score = -quiesce( pos ,-beta, -alpha );
            pos.undoMove();
            if( score >= beta )
                return beta;
            if( score > alpha ){
                alpha = score; 
            }
        }
    }
    return alpha;
}



// EVALUATION FUNCTION

const int pieceValues[12] = {100, 300, 300, 500, 900, 10000, -100, -300, -300, -500, -900, -10000};

// ALL SCORES ARE FROM BLACK'S PERSPECTIVE
const int pawn_score[64] = 
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] = 
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  20,   0,  10,  10,   0,  20,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0
};

// rook positional score
const int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirriring square values for white
const int mirror[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
    8,  9, 10, 11, 12, 13, 14, 15,
    0,  1,  2,  3,  4,  5,  6,  7
};

const int getRank[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7
};

const int getFile[64] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7
};


const int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 }; 

int Engine::evaluatePosition(Position pos){
    int score = 0;
    Color sideToMove = pos.getSideToMove();
    Bitboard currentBoard;

    int square,file,dubblets;

    for (PieceType p = wPAWN; p < NO_PIECE; p++){
        currentBoard = pos.getPieceBitboard(p);
        while (currentBoard){
            square = std::countr_zero(currentBoard);
            currentBoard ^= (1ULL << square);

            score += pieceValues[p];

            switch (p){
            case wPAWN:
                score += pawn_score[square]; // POSITIONAL SCORE

                file = getFile[square];
                dubblets = std::popcount(pos.getPieceBitboard(wPAWN) & FILES[file]);
                if (dubblets > 1){                      // DUBBLE PAWNS PENALTY
                    score += dubblets * (-10);
                }
                
                // Check if pawn for isolated pawn
                if (file == 0){
                    if (!(pos.getPieceBitboard(wPAWN) & FILES[file + 1])){
                        score -= 10;
                    }
                }
                else if (file == 7){
                    if (!(pos.getPieceBitboard(wPAWN) & FILES[file - 1])){
                        score -= 10;
                    }
                }
                else{
                    if (!(pos.getPieceBitboard(wPAWN) & FILES[file + 1]) && !(pos.getPieceBitboard(wPAWN) & FILES[file - 1])){
                        score -= 10;
                    }
                }

                // Check if pawn is passed
                if (file == 0){
                    if(!(pos.getPieceBitboard(bPAWN) & FILES[file + 1]) && !(pos.getPieceBitboard(bPAWN) & FILES[file])){
                        score += passed_pawn_bonus[7 - getRank[square]];
                    }
                }
                else if (file == 7){
                    if(!(pos.getPieceBitboard(bPAWN) & FILES[file - 1]) && !(pos.getPieceBitboard(bPAWN) & FILES[file])){
                        score += passed_pawn_bonus[7 - getRank[square]];
                    }
                }
                else{
                    if(!(pos.getPieceBitboard(bPAWN) & FILES[file + 1]) && !(pos.getPieceBitboard(bPAWN) & FILES[file - 1]) && !(pos.getPieceBitboard(bPAWN) & FILES[file])){
                        score += passed_pawn_bonus[7 - getRank[square]];
                    }
                }
                break;
            case wKNIGHT:
                score += knight_score[square]; // POSITIONAL SCORE
                break;
            case wBISHOP:
                score += bishop_score[square]; // POSITIONAL SCORE

                score += std::popcount(getBishopAttacks(square, pos.getOccupiedSquaresBitboard())); // MOBILITY SCORE
                break;
            case wROOK:
                score += rook_score[square]; // POSITIONAL SCORE

                score += std::popcount(getRookAttacks(square, pos.getOccupiedSquaresBitboard())); // MOBILITY SCORE

                // Check if rook is on open file
                if(!(pos.getPieceBitboard(wPAWN) & FILES[getFile[square]])){
                    score += 10;
                }
                if(!((pos.getPieceBitboard(wPAWN) | pos.getPieceBitboard(bPAWN)) & FILES[getFile[square]])){
                    score += 15;
                }
                break;
            case wQUEEN:
                    score += std::popcount(getRookAttacks(square, pos.getOccupiedSquaresBitboard()) | getBishopAttacks(square, pos.getOccupiedSquaresBitboard())); // MOBILITY SCORE
                break;
            case wKING:
                score += king_score[square]; // POSITIONAL SCORE

                // Check if king is on open file penalty
                if(!(pos.getPieceBitboard(wPAWN) & FILES[getFile[square]])){
                    score -= 10;
                }
                if(!((pos.getPieceBitboard(wPAWN) | pos.getPieceBitboard(bPAWN)) & FILES[getFile[square]])){
                    score -= 15;
                }

                score += std::popcount(kingAttacks[square] & pos.getAllPiecesBitboard(WHITE)) * 5; // KING SAFETY SCORE

                break;

            case bPAWN:
                score -= pawn_score[mirror[square]];
                
                file = getFile[square];
                dubblets = std::popcount(pos.getPieceBitboard(bPAWN) & FILES[file]);
                if (dubblets > 1){                      // DUBBLE PAWNS PENALTY
                    score -= dubblets * (-10);
                }

                // Check if pawn for isolated pawn
                if (file == 0){
                    if (!(pos.getPieceBitboard(bPAWN) & FILES[file + 1])){
                        score += 10;
                    }
                }
                else if (file == 7){
                    if (!(pos.getPieceBitboard(bPAWN) & FILES[file - 1])){
                        score += 10;
                    }
                }
                else{
                    if (!(pos.getPieceBitboard(bPAWN) & FILES[file + 1]) && !(pos.getPieceBitboard(bPAWN) & FILES[file - 1])){
                        score += 10;
                    }
                }

                // Check if pawn is passed
                if (file == 0){
                    if(!(pos.getPieceBitboard(wPAWN) & FILES[file + 1]) && !(pos.getPieceBitboard(wPAWN) & FILES[file])){
                        score -= passed_pawn_bonus[getRank[square]];
                    }
                }
                else if (file == 7){
                    if(!(pos.getPieceBitboard(wPAWN) & FILES[file - 1]) && !(pos.getPieceBitboard(wPAWN) & FILES[file])){
                        score -= passed_pawn_bonus[getRank[square]];
                    }
                }
                else{
                    if(!(pos.getPieceBitboard(wPAWN) & FILES[file + 1]) && !(pos.getPieceBitboard(wPAWN) & FILES[file - 1]) && !(pos.getPieceBitboard(wPAWN) & FILES[file])){
                        score -= passed_pawn_bonus[getRank[square]];
                    }
                }

                break;
            case bKNIGHT:
                score -= knight_score[mirror[square]];
                break;
            case bBISHOP:
                score -= bishop_score[mirror[square]];

                score -= std::popcount(getBishopAttacks(square, pos.getOccupiedSquaresBitboard())); // MOBILITY SCORE
                break;
            case bROOK:
                score -= rook_score[mirror[square]];

                score -= std::popcount(getRookAttacks(square, pos.getOccupiedSquaresBitboard())); // MOBILITY SCORE

                // Check if rook is on open file
                if(!(pos.getPieceBitboard(bPAWN) & FILES[getFile[square]])){
                    score -= 10;
                }
                if(!((pos.getPieceBitboard(wPAWN) | pos.getPieceBitboard(bPAWN)) & FILES[getFile[square]])){
                    score -= 15;
                }
                break;
            case bQUEEN:
                score -= std::popcount(getRookAttacks(square, pos.getOccupiedSquaresBitboard()) | getBishopAttacks(square, pos.getOccupiedSquaresBitboard())); 
                break;
            case bKING:
                score -= king_score[mirror[square]];

                // Check if king is on open file penalty
                if(!(pos.getPieceBitboard(bPAWN) & FILES[getFile[square]])){
                    score += 10;
                }
                if(!((pos.getPieceBitboard(wPAWN) | pos.getPieceBitboard(bPAWN)) & FILES[getFile[square]])){
                    score += 15;
                }

                score -= std::popcount(kingAttacks[square] & pos.getAllPiecesBitboard(BLACK)) * 5; // KING SAFETY SCORE
                break;
            
            default:
                break;
            }
        }
            
    }
    
    return sideToMove == WHITE ? score : -score;
}