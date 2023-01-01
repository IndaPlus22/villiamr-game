#include "Engine.hpp"

Engine::Engine(int depth){
    this->maxDepth = depth;
    this->stop = false;

    this->zobrist = Zobrist();
}

void moveToString(move m)
{
    const std::string squares[64] = {"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
                                     "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
                                     "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
                                     "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
                                     "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
                                     "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
                                     "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
                                     "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

    if (getMoveType(m) == PROMOTON || getMoveType(m) == PROMOTION_CAPTURE)
    {
        std::cout << squares[getFromSquare(m)] << squares[getToSquare(m)] << "q";
        return;
    }
    std::cout << squares[getFromSquare(m)] << squares[getToSquare(m)];
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

// Killer moves - quiet moves that cause beta cutoff in search
move killerMoves[2][64]; // ID , plys or halfmoves

// history moves 
move historyMoves[12][64]; // piecetype, sqares

 // PV TABLE
// Pincipal variation is most likely and perforable variation the engine wants to play
int pvLength[64]; 
move pvTable[64][64];

bool followPV;
bool scorePV;
const int maxPly = 64;



int scoreMove(move m,Position pos){
    MoveType type = getMoveType(m);

    if(pvTable[0][pos.getHalfMoveCounter()] == m && scorePV){
        scorePV = false;
        return 20000;
    }
    if (type == CAPTURE){
        return mvv_lva[pos.getPieceType(getFromSquare(m))][pos.getPieceType(getToSquare(m))] + 10000;
    }
    else if (type == EN_PASSANT){
        return mvv_lva[wPAWN][wPAWN] + 10000;
    }

    else if(killerMoves[0][pos.getHalfMoveCounter()] == m)
        return 9000;

    else if(killerMoves[1][pos.getHalfMoveCounter()] == m)
        return 8000;    

    else 
        return historyMoves[pos.getPieceType(getFromSquare(m))][getToSquare(m)];
}


void sortMoves(std::vector<move> &movelist, Position obj){
    int moveScores[movelist.size()];

    for (int i = 0; i < movelist.size(); i++){
        moveScores[i] = scoreMove(movelist[i],obj);
    }

    //Sort the moves by putting all zero scores at the end and then sorting the rest
    int zeroIndex = 0;
    for(int i = 0; i < movelist.size(); i++){
        if(moveScores[i] != 0){
            std::swap(movelist[i],movelist[zeroIndex]);
            std::swap(moveScores[i],moveScores[zeroIndex]);
            zeroIndex++;
        }
    }
    
    for (int i = 0; i < zeroIndex; i++){
        for (int j = i+1; j < zeroIndex; j++){
            if (moveScores[i] < moveScores[j]){
                std::swap(movelist[i],movelist[j]);
                std::swap(moveScores[i],moveScores[j]);
            }
        }
    }
}

void enablePVscoring(std::vector<move> movelist,Position pos){
    followPV = false;
    for (move m : movelist){
        if (pvTable[0][pos.getHalfMoveCounter()] == m){
            followPV = true;
            scorePV = true;
        }
    }

}

void Engine::findBestMove(Position pos){
    std::chrono::duration<double> executiontime;
    nodes = 0;
    followPV = false;
    scorePV = false;
    pos.resetHalfMove();
    memset(killerMoves,0,sizeof(killerMoves));
    memset(historyMoves,0,sizeof(historyMoves));
    memset(pvTable,0,sizeof(pvTable));
    memset(pvLength,0,sizeof(pvLength));

    int alpha = -50000;
    int beta = 50000;
    
    for (int dep = 1; dep <= maxDepth && !stop ;dep++){
        followPV = true;
        auto start = std::chrono::high_resolution_clock::now();

        int score = minimax(pos, dep, alpha, beta);
        
        auto end = std::chrono::high_resolution_clock::now();
        executiontime = end - start;

        // Aspiration window
        if(score <= alpha || score >= beta){ // if score is out of bounds we have to do costly re-search
            alpha = -50000;
            beta = 50000;
            dep--;
            continue;
        }
        alpha = score - 50;
        beta = score + 50;
        
           

        std::cout << "info score cp " << score << " nodes " << nodes << 
                                                                            " depth " << dep << 
                                                                            " time " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << 
                                                                            " nps " << nodes / executiontime.count() << 
                                                                            " pv";

            for (int i = 0; i < pvLength[0]; i++){
                std::cout << " ";
                moveToString(pvTable[0][i]);
            }
            
            std::cout << std::endl;    
    }
    std::cout << "bestmove ";
    moveToString(pvTable[0][0]);
    std::cout << std::endl;
    pos.makeMove(pvTable[0][0]);
    
    
}


/*  
    MINIMAX algorithm with alpha-beta pruning to find the best move
*/
const int fullDepthMoves = 4;
const int reductionLimit = 3;
bool nullPruning = false;
int Engine::minimax(Position pos,int depth, int alpha, int beta){
    if( depth == 0 ) return quiesce(pos,alpha,beta);

    if(pos.getHalfMoveCounter() >= 63){ // Protect against going too deep in precence of perpetual check
        return evaluatePosition(pos);
    }

    pvLength[pos.getHalfMoveCounter()] = pos.getHalfMoveCounter();
    int score;


    // INCREASE DEPTH IF IN CHECK
    bool inCheck = getCheckers(pos,pos.getSideToMove()); // Bitboard used as a boolean value
    if(inCheck){ // If check we need to go deeper
         depth++;
    }

    // NULL MOVE PRUNING
    if(!inCheck && depth >= reductionLimit && pos.getHalfMoveCounter()){ // Don't prune if in root or check
        nullPruning = true; 
        pos.toggleSideToMove();
        pos.nullEnPassantSquare();
        score = -minimax(pos,depth-3,-beta,-beta+1);
        pos.toggleSideToMove();
        nullPruning = false;
        if(score >= beta)
            return beta;
    }


    // SORTING MOVES
    std::vector<move> moves = generateLegalMoves(pos);
    if(followPV)
        enablePVscoring(moves,pos);
    sortMoves(moves,pos);

    int searchedMoves = 0;

    for ( move m : moves ) {
        pos.makeMove(m);
        nodes++;

        // LATE MOVE REDUCTION (LMR)
       
        // Full depth
        if(searchedMoves == 0)
            score = -minimax(pos,depth-1, -beta, -alpha);
        // LATE MOVE REDUCTION
        else {
            if(searchedMoves >= fullDepthMoves && depth >= reductionLimit && !inCheck && getMoveType(m) != CAPTURE && getMoveType(m) != PROMOTON && getMoveType(m) != PROMOTION_CAPTURE){
                score = -minimax(pos,depth -2,-alpha-1,-alpha);
            }
            else 
                score = alpha +1; // Hack that forces a full search if LMR fails
            
            // PRIINCIPAL VARIATION SEARCH
            if(score > alpha){
                score = -minimax(pos,depth-1,-alpha -1,-alpha); // Full search reduced bandwith
                if(score > alpha && score < beta){
                    score = -minimax(pos,depth-1,-beta,-alpha); // If all lmrs fail do a full search
                }
            }
        }
        
        pos.undoMove();
        searchedMoves++;

        if( score >= beta ){
            //KILLER MOVE HEURISTIC
            if(getMoveType(m) != CAPTURE && getMoveType(m) != PROMOTION_CAPTURE){
                killerMoves[1][pos.getHalfMoveCounter()] = killerMoves[0][pos.getHalfMoveCounter()];
                killerMoves[0][pos.getHalfMoveCounter()] = m;
            }

            return beta;   //  fail hard beta-cutoff
        }
        if( score > alpha ){
            alpha = score; // alpha acts like max in MiniMax
            

            // PRINCIPAL VARIATION TABLE
            if(!nullPruning){ // Only allow pv moves if not in null move pruning
                pvTable[pos.getHalfMoveCounter()][pos.getHalfMoveCounter()] = m;
                // copy deeper moves to current ply
                for (int p = pos.getHalfMoveCounter() + 1; p < pvLength[pos.getHalfMoveCounter() + 1]; p++){
                    pvTable[pos.getHalfMoveCounter()][p] = pvTable[pos.getHalfMoveCounter() + 1][p];
                }

                pvLength[pos.getHalfMoveCounter()] = pvLength[pos.getHalfMoveCounter() + 1];
            }

            // HISTORY HEURISTIC
            if(getMoveType(m) != CAPTURE && getMoveType(m) != PROMOTION_CAPTURE)
                historyMoves[pos.getPieceType(getFromSquare(m))][getToSquare(m)] += depth;
            
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
        if ((getMoveType(m) == CAPTURE) || getMoveType(m) == PROMOTION_CAPTURE){
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