#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include <vector>
#include <bit>

#include "types.hpp"
#include "position.hpp"
#include "magicConstants.hpp"

std::vector<move> generateLegalMoves(Position pos);

Bitboard getCheckers(Position pos, Color sideToMove);
#endif