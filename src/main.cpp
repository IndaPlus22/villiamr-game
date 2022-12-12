#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<filesystem>
#include<iostream>
#include <string>
#include <chrono>

#include "graphicsBase.hpp"
#include "position.hpp"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

int getSquare (int x, int y){
    return (x / 125) + (y / 125) * 8;
}

int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Chess", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    GraphicsBase graphicsBase(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    Position position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


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