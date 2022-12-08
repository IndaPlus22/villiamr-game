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
    return (x % 8) * (y / 8);
}

int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Chess", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    //GraphicsBase graphicsBase(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    Position position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


    bool quit = false;
    while (!quit){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            std::cout << "Event: " << event.type << std::endl;
            if (event.type == SDL_QUIT){
                quit = true;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    

    


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}