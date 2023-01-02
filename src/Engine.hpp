#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <cstring>
#include "position.hpp"
#include "movegen.hpp"

#define HASHSIZE 0x500000 // TRANSPOSITION TABLE size 5.2MB
#define unknownScore 1000000 // score for unknown positions

// TRANSPOSITION TABLE flags
typedef enum HASHFLAG{
    EXACT,
    ALPHA,
    BETA,
    NONE
} HashFlag;

// TRANSPOSITION TABLE entry
typedef struct HASHENTRY{
    Bitboard hash;
    int depth;
    int score;
    HashFlag flag;
}HashEntry;

class Engine{
private:
    std::vector<HashEntry> transpositionTable;

    int maxDepth;
    Bitboard nodes;

    int quiesce(Position pos,int alpha, int beta);
    int minimax(Position position ,int depth, int alpha, int beta);

    int ProbeTT(Bitboard hash, int depth, int alpha, int beta);
    void StoreTT(Bitboard hash, int depth, int score, HashFlag flag);

public:
    Engine() = default;
    Engine(int depth);

    void clearTT();


    void findBestMove(Position position);
    Bitboard getNodes() const {return nodes;};

    //void setStop(bool stop) {this->stop = stop;};

    int evaluatePosition(Position position);
};

#endif