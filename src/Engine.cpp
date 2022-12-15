#include "Engine.hpp"

Engine::Engine(PieceColor playerColor, int depth){
    this->playerColor = playerColor;
    this->maxDepth = depth;
}

/*  
    MINIMAX algorithm with alpha-beta pruning to find the best move
*/
int Engine::minimax(Position pos,int depth, int alpha, int beta){
    if (depth == 0){
        return evaluatePosition(pos);
    }

    pos.generateLegalMoves();
    std::vector<Cmove> moves = pos.getLegalMoves();
    for (Cmove c: moves){
        pos.makeMove(c);
        int score = -minimax(pos, depth-1, -beta, -alpha);
        pos.unmakeMove();
        if (score >= beta){
            return beta;
        }
        if (score > alpha){
            alpha = score;
            if (depth == maxDepth){
                bestMove = c;
            }
        }
    }
    return alpha;
}



/*
    Evaluation of the position 
*/



const int pieceValues[7] = {1, 3, 3, 5, 9, 10000};
int materialScore (Position pos){
    int score = 0;
    score += pieceValues[0] * std::popcount(pos.getBitboard(wPAWN));
    score += pieceValues[1] * std::popcount(pos.getBitboard(wKNIGHT));
    score += pieceValues[2] * std::popcount(pos.getBitboard(wBISHOP));
    score += pieceValues[3] * std::popcount(pos.getBitboard(wROOK));
    score += pieceValues[4] * std::popcount(pos.getBitboard(wQUEEN));
    score += pieceValues[5] * std::popcount(pos.getBitboard(wKING));

    score -= pieceValues[0] * std::popcount(pos.getBitboard(bPAWN));
    score -= pieceValues[1] * std::popcount(pos.getBitboard(bKNIGHT));
    score -= pieceValues[2] * std::popcount(pos.getBitboard(bBISHOP));
    score -= pieceValues[3] * std::popcount(pos.getBitboard(bROOK));
    score -= pieceValues[4] * std::popcount(pos.getBitboard(bQUEEN));
    score -= pieceValues[5] * std::popcount(pos.getBitboard(bKING));
    return score;
}

int developmentScore(Position pos){
    int score = 0;
    for (PieceType p = wPAWN; p < NO_PIECE; p++){
        if ( p < bPAWN )
            score += std::popcount(pos.getBitboard(p) & ~pos.getOriginalBitboard(p));
        else
            score -= std::popcount(pos.getBitboard(p) & ~pos.getOriginalBitboard(p));
    }
    return score;
}

int mobilityScore(Position pos){
    int score = 0;
    if (pos.getSideToMove() == WHITE){
        score += pos.getLegalMoves().size();
        pos.setSideToMove(BLACK);
        score -= pos.getLegalMoves().size();
        pos.setSideToMove(WHITE);
        return score;
    }

    score -= pos.getLegalMoves().size();
    pos.setSideToMove(WHITE);
    score += pos.getLegalMoves().size();
    pos.setSideToMove(BLACK);
    return score;
}

int centralScore(Position pos){
    int score = 0;
    Bitboard central = 0x183C3C180000;
    score += std::popcount(pos.getColorBitboard(WHITE) & central);
    score -= std::popcount(pos.getColorBitboard(BLACK) & central);
    return score;
}

int attackScore(Position pos){
    int score = 0;
    if (pos.getSideToMove() == WHITE){
        score += std::popcount(pos.returnAttackboard());
        pos.setSideToMove(BLACK);
        score -= std::popcount(pos.returnAttackboard());
        pos.setSideToMove(WHITE);
        return score;
    }

    score -= std::popcount(pos.returnAttackboard());
    pos.setSideToMove(WHITE);
    score += std::popcount(pos.returnAttackboard());
    pos.setSideToMove(BLACK);
    return score;
}

int Engine::evaluatePosition(Position pos){
    int score = 0;
    int materialWeight = 4;
    int mobilityWeight = 3;
    int centralWeight = 2;
    int developmentWeight = 3;
    int attackWeight = 2;

    score += materialWeight * materialScore(pos);
    score += mobilityWeight * mobilityScore(pos);
    score += centralWeight * centralScore(pos);
    score += developmentWeight * developmentScore(pos);
    score += attackWeight * attackScore(pos);
    return score;
}

void Engine::findBestMove(Position pos){
    minimax(pos, maxDepth, -INT32_MAX, INT32_MAX);
}

