# CPP chess engine

This is a chess engine build in cpp. Movegeneration is based on bitboards and features [`magic bitboards`](https://www.chessprogramming.org/Magic_Bitboards) and [setwise pawn operations](https://www.chessprogramming.org/Pawn_Pattern_and_Properties). All slider moves are generated at initialization then available thru hasing with square and occupancy bitboard. And pawn moves are generated on the fly by bitshfting the set of pawns on a bitboard. 

The engine uses minimax with alpha beta pruning to find best move. Some optimizations for this search routine include move ordering by (Principal variation, Caputres LVV_MVT, Killer moves and history heuristic), [Principal variation search](https://www.chessprogramming.org/Principal_Variation_Search), [Late move reduction](https://www.chessprogramming.org/Late_Move_Reductions), [Null move pruning](https://www.chessprogramming.org/Null_Move_Pruning) and a [Transposition table](https://www.chessprogramming.org/Transposition_Table) via zobrist hashing. 

The engine runs via the UCI protocol so it is recomended to run thru any uci [gui](https://www.chessprogramming.org/UCI#guis).


## Building and running

Project is built using cmake and run via UCI gui (recomended [Cute chess](https://www.chessprogramming.org/Cute_Chess)).

Script for building, in the `villiamr-game` folder run the following:

```bash
mkdir build/;cd build/; cmake -DCMAKE_BUILD_TYPE=Release ..;make all 
```

Exebutable is found in `villiamr-game/build/src`. 

### Setting up gui

In `tools > settings > engines` press te plus icon and for the `command` option browse to engine executable.

Then you just press `game > new` and cpu with Chess as white or black player.


