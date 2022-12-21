#include "position.hpp"
#include "graphicsBase.hpp"
#include "movegen.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

Bitboard perft(int depth, Position position, GraphicsBase& graphics){
    if (depth == 0){
        return 1;
    }

    Bitboard nodes = 0;
    std::vector<move> moves = generateLegalMoves(position); 
    for (move m : moves){
        position.makeMove(m);
        //graphics.drawBoard(position, {});
        nodes += perft(depth - 1, position, graphics);
        position.undoMove();
        //graphics.drawBoard(position, {});
    }
    return nodes;
}

int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    GraphicsBase graphics(800, 800);


    Bitboard nodes = perft(5,pos,graphics);
    std::cout << nodes << std::endl;

/*
    std::vector<int> highlightedSquares;
    bool running = true;
    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN){
                int x, y;
                SDL_GetMouseState(&x, &y);
                int square = (x / 100) + (y / 100) * 8;
                highlightedSquares.push_back(square);
                if( highlightedSquares.size() == 2){
                    for (move m : moves){
                        if (getFromSquare(m) == highlightedSquares[0] && getToSquare(m) == highlightedSquares[1]){
                            pos.makeMove(m);
                            graphics.drawBoard(pos, {});
                            moves = generateLegalMoves(pos);
                            break;
                        }
                    }
                    highlightedSquares.clear();
                }
                else {
                    highlightedSquares.clear();
                    for (move m: moves){
                        if (getFromSquare(m) == highlightedSquares[0]){
                            highlightedSquares.push_back(getToSquare(m));
                        }
                    }
                }
                
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_u){
                    pos.undoMove();
                    moves = generateLegalMoves(pos);
                    graphics.drawBoard(pos, {});
                }
            }
        }
        graphics.drawBoard(pos, highlightedSquares);
    }
*/
    graphics.close();
    return 0;
}