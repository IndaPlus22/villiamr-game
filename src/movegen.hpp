#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include <vector>
#include <bit>
#include <algorithm>

#include "types.hpp"
#include "position.hpp"
#include "magicConstants.hpp"

std::vector<move> generateLegalMoves(Position &pos);

Bitboard getAttackboard(Position pos, Color generatingside);

Bitboard getBishopAttacks(int square, Bitboard occupancy);

Bitboard getRookAttacks(int square, Bitboard occupancy);

#endif