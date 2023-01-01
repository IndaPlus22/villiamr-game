#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <cstring>
#include "position.hpp"
#include "movegen.hpp"
#include "zobrist.hpp"


class Engine{
private:
    Zobrist zobrist;

    int maxDepth;
    Bitboard nodes;

    int quiesce(Position pos,int alpha, int beta);
    int minimax(Position position ,int depth, int alpha, int beta);

    bool stop;

public:
    Engine() = default;
    Engine(int depth);

   


    void findBestMove(Position position);
    Bitboard getNodes() const {return nodes;};

    void setStop(bool stop) {this->stop = stop;};

    int evaluatePosition(Position position);
};

// TODO: IMPLEMENT ZOBRIST HASHING and MOVETYPE CHECKS TO ALLOW QUIESCENCE SEARCH TO BE MORE EFFECTIVE (AND MAYBE SEE CHECKMATES)

#endif