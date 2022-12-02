#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<filesystem>
#include<iostream>
#include <string.h>

#include "graphicsBase.hpp"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000


int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    GraphicsBase graphicsBase(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);



    SDL_RenderClear(renderer);

    graphicsBase.drawBase();  

    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}