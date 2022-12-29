#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include "position.hpp"
#include "movegen.hpp"


class Engine{
private:
    int maxDepth;
    int bestscore;

    move bestMove;
    Bitboard nodes;

    std::vector<int> weights;


    int quiesce(Position pos,int alpha, int beta);
    int minimax(Position position ,int depth, int alpha, int beta);

public:
    Engine(int depth);

    int getBestScore() const {return bestscore;};

    void findBestMove(Position position, std::chrono::duration<double> &executionTime);
    move getEngineMove() const {return bestMove;};
    Bitboard getNodes() const {return nodes;};


    int evaluatePosition(Position position);
};

// TODO: IMPLEMENT ZOBRIST HASHING and MOVETYPE CHECKS TO ALLOW QUIESCENCE SEARCH TO BE MORE EFFECTIVE (AND MAYBE SEE CHECKMATES)

#endif