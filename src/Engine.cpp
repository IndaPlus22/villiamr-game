#include "Engine.hpp"

Engine::Engine(Color playerColor, int depth){
    this->playerColor = playerColor;
    this->maxDepth = depth;
    this->bestMove = 0;
}

void Engine::findBestMove(Position pos){
    std::cout << "Finding best move..." << std::endl;
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
                bestMove = m;
            }
        }
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
    std::vector<int> pieceValues = {1, 3, 3, 5, 9, 100, -1, -3, -3, -5, -9, -100};
    for (PieceType p = wPAWN; p < NO_PIECE; p++){
        score += pieceValues[p] * std::popcount(pos.getPieceBitboard(p));
    }
    Bitboard center = 0x00'00'3c'3c'3c'3c'00'00;
    score += std::popcount(pos.getAllPiecesBitboard(WHITE) & center);
    score -= std::popcount(pos.getAllPiecesBitboard(BLACK) & center);

    Bitboard originalPos = 0xFF;
    Bitboard originalPos2 = (0xFFULL << 56);
    score -= std::popcount(pos.getAllPiecesBitboard(WHITE) & originalPos) * 2;
    score += std::popcount(pos.getAllPiecesBitboard(BLACK) & originalPos2) * 2;

    if (pos.getCheckmate()){
        pos.getSideToMove() == WHITE ? score -= INT16_MAX : score += INT16_MAX;
    }
    return pos.getSideToMove() == WHITE ? score : -score;
}