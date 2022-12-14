# CPP chess engine

This is a chess engine build in cpp. Movegeneration is based upon bitboards and features [`magic bitboards`](https://www.chessprogramming.org/Magic_Bitboards) and [setwise pawn operations](https://www.chessprogramming.org/Pawn_Pattern_and_Properties). All slider moves are generated at initialization then available thru hasing with square and occupancy bitboard. And pawn moves are generated on the fly by bitshfting the set of pawns on a bitboard. [Perft](https://www.chessprogramming.org/Perft) testing on release build in fig below:

| Depth | Nodes | Execution time (s) | NPS |
|-------|-------|--------------------|-----|
| 1     | 20    | 7.9e-6             | 2.5e+6 |
| 2     | 414   | 6.2e-5             | 6.7e+6 |
| 3     | 9510  | 0.001              | 9.3e+6 |
| 4     | 214844| 0.03               | 7.2e+6 |
| 5     | 5400859| 0.6               | 8.8e+6 |
| 6     | 133600172 | 19.7           | 6.8e+6 |

## Building and running

Project is built using cmake. Rendering is slightly buggy for non native builds like thur WSL but works. 

Script for building, in the `villiamr-game` folder run the following:

```bash
mkdir build/;cd build/; cmake -DCMAKE_BUILD_TYPE=Release ..;make all 
```

Exebutable is found in `villiamr-game/build/src`. 


