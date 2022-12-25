#include "Engine.hpp"

Engine::Engine(Color playerColor, int depth){
    this->playerColor = playerColor;
    this->maxDepth = depth;
    this->bestMove = 0;
    this->bestscore = 0;
}

void Engine::findBestMove(Position pos){
    bestMove = generateLegalMoves(pos)[0];
    minimax(pos, maxDepth, -INT16_MAX, INT16_MAX);
}

/*  
    MINIMAX algorithm with alpha-beta pruning to find the best move
*/
int Engine::minimax(Position pos,int depth, int alpha, int beta){
    if( depth == 0 ) return quiesce(pos,alpha,beta);

    std::vector<move> moves = generateLegalMoves(pos);
    for ( move m : moves ) {
        pos.makeMove(m);
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
    for ( move m : moves ) {
        if ((getMoveType(m) == CAPTURE || getMoveType(m) == PROMOTON || getMoveType(m) == PROMOTION_CAPTURE) && (pos.getPieceType(getToSquare(m)) != wPAWN || pos.getPieceType(getToSquare(m)) != bPAWN)){
            pos.makeMove(m);
            int score = -quiesce( pos ,-beta, -alpha );
            pos.undoMove();
            if( score >= beta )
                return beta;
            // DELTA PRUNING prevents quiescence search from searching moves that are not likely to be good
            int BIG_DELTA = 9; // Queen value
            if(getMoveType(m) == PROMOTON || getMoveType(m) == PROMOTION_CAPTURE){
                BIG_DELTA += 7; // Greater than rook
            }
            if( score > alpha - BIG_DELTA ){
                return alpha;
            }
            if( score > alpha ){
                alpha = score; 
            }
        }
    }
    return alpha;
}




int Engine::evaluatePosition(Position pos){
    int score = 0;

    Color sideToMove = pos.getSideToMove();
    std::vector<int> pieceValues = {1, 3, 3, 5, 9, INT16_MAX, -1, -3, -3, -5, -9, INT16_MIN};
    for (PieceType p = wPAWN; p < NO_PIECE; p++){
        score += pieceValues[p] * std::popcount(pos.getPieceBitboard(p));
    }

    Bitboard whiteAttackboard = getAttackboard(pos,WHITE);
    Bitboard blackAttackboard = getAttackboard(pos,BLACK);

    score += (std::popcount(whiteAttackboard & ~pos.getAllPiecesBitboard(WHITE)) / 2) + std::popcount(whiteAttackboard & pos.getAllPiecesBitboard(BLACK)); 
    score -= (std::popcount(blackAttackboard & ~pos.getAllPiecesBitboard(BLACK)) / 2) + std::popcount(blackAttackboard & pos.getAllPiecesBitboard(WHITE));

    Bitboard center = 0x0000001818000000;
    score += std::popcount(pos.getAllPiecesBitboard(WHITE) & center) / 2;
    score -= std::popcount(pos.getAllPiecesBitboard(BLACK) & center) / 2;

    Bitboard whitepawnPromotionIncentive = 0x000000000000FF00;
    Bitboard blackpawnPromotionIncentive = 0x00FF000000000000;
    score += std::popcount(pos.getPieceBitboard(wPAWN) & whitepawnPromotionIncentive) * 10;
    score -= std::popcount(pos.getPieceBitboard(bPAWN) & blackpawnPromotionIncentive) * 10;


    
    return sideToMove == WHITE ? score : -score;
}