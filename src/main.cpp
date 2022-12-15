#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<filesystem>
#include<iostream>
#include <string>
#include <chrono>

#include "graphicsBase.hpp"
#include "position.hpp"
#include "Engine.hpp"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

int getSquare (int x, int y){
    return (x / (SCREEN_WIDTH / 8)) + (y / (SCREEN_HEIGHT / 8)) * 8;
}


Bitboard perft(int depth, Position position){
    if (depth == 0){
        return 1;
    }

    Bitboard nodes = 0;
    position.generateLegalMoves();
    std::vector<Cmove> moves = position.getLegalMoves();
    for (Cmove move : moves){
        position.makeMove(move);
        nodes += perft(depth - 1, position);
        position.unmakeMove();
    }
    return nodes;
}

int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Chess", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    GraphicsBase graphicsBase(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    Position position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    Engine engine(BLACK, 5);


    bool quit = false;
    bool movemade = true;
    enum state {select, move};
    state currentState = select;
    std::vector<int> highlightedSquares;
    std::vector<Cmove> moves;

    while (!quit){
        SDL_Event event;
        if(movemade){
            position.generateLegalMoves();
            moves = position.getLegalMoves();
            if(moves.size() == 0){
                quit = true;
            }
            movemade = false;
            if (position.getSideToMove() == BLACK){
                engine.findBestMove(position);
                std::cout << engine.getEngineMove().getFrom() << " " << engine.getEngineMove().getTo() << std::endl;
                position.makeMove(engine.getEngineMove());
                movemade = true;
                SDL_RenderClear(renderer);
                graphicsBase.drawPosition(position);
                SDL_RenderPresent(renderer);
                position.generateLegalMoves();
                moves = position.getLegalMoves();
            }
        }
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                quit = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN){
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (currentState == select){
                    highlightedSquares.clear();
                    highlightedSquares.push_back(getSquare(x, y));
                    for (Cmove move : moves){
                        if (move.getFrom() == getSquare(x, y)){
                            highlightedSquares.push_back(move.getTo());
                        }
                    }
                    currentState = move;
                    SDL_RenderClear(renderer);
                    graphicsBase.drawPosition(position);
                    for (int square : highlightedSquares){
                        graphicsBase.highlightSquare(square);
                    }
                    SDL_RenderPresent(renderer);
                }
                else if (currentState == move){
                    for (Cmove move : moves){
                        if (move.getFrom() == highlightedSquares[0] && move.getTo() == getSquare(x, y)){
                            position.makeMove(move);
                            movemade = true;
                            currentState = select;
                        }
                    }
                    highlightedSquares.clear();
                    currentState = select;
                    SDL_RenderClear(renderer);
                    graphicsBase.drawPosition(position);
                    SDL_RenderPresent(renderer);
                }
            }
            if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.sym == SDLK_u){
                    position.unmakeMove();
                    movemade = true;
                }
                if(event.key.keysym.sym == SDLK_p){
                    int depth = 5;
                    std::cout << "*****Starting perft search at depth: " << depth << "*****" << std::endl;
                    std::cout << "Searching..." << std::endl;

                    auto start = std::chrono::high_resolution_clock::now();
                    Bitboard nodes = perft(depth, position);
                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = end - start;

                    std::cout << "\33[A\33[2K\33[A\33[2K\r*****Perft search complete*****" << std::endl; 
                    std::cout << "Nodes searched: "<< nodes << std::endl;
                    std::cout << "Time: " << elapsed.count() << std::endl;
                    std::cout << "Nodes per second: " << nodes / elapsed.count() << std::endl;
                }
            }
        }
        //SDL_RenderClear(renderer);
        //graphicsBase.drawPosition(position);
        //for (int square : highlightedSquares){
        //    graphicsBase.highlightSquare(square);
        //}
        //SDL_RenderPresent(renderer);
    }
    

    


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}