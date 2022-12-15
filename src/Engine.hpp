#ifndef ENGINE
#define ENGINE

#include <cmath>
#include "position.hpp"


class Engine{
private:
    PieceColor playerColor;
    int maxDepth;

    Cmove bestMove;

    int evaluatePosition(Position position);
    int minimax(Position position ,int depth, int alpha, int beta);

public:
    Engine(PieceColor playerColor, int depth);

    void findBestMove(Position position);
    Cmove getEngineMove() const {return bestMove;};
};

#endif