#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "position.hpp"
#include "movegen.hpp"


class Engine{
private:
    Color playerColor;
    int maxDepth;

    move bestMove;

    int evaluatePosition(Position position);
    int quiesce(Position pos,int alpha, int beta);
    int minimax(Position position ,int depth, int alpha, int beta);

public:
    Engine(Color playerColor, int depth);

    Color getPlayerColor() const {return playerColor;};

    void findBestMove(Position position);
    move getEngineMove() const {return bestMove;};
};

// TODO: IMPLEMENT ZOBRIST HASHING and MOVETYPE CHECKS TO ALLOW QUIESCENCE SEARCH TO BE MORE EFFECTIVE (AND MAYBE SEE CHECKMATES)

#endif