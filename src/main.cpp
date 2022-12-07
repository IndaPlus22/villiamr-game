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


int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    GraphicsBase graphicsBase(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    Position position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

    position.generateLegalMoves();
    position.makeMove(Cmove(12, 20, NORMAL));
    position.generateLegalMoves();

    SDL_RenderClear(renderer);
    graphicsBase.drawPosition(position);
    SDL_RenderPresent(renderer);


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}