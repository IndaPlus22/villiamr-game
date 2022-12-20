#include "position.hpp"
#include "graphicsBase.hpp"
#include <SDL2/SDL.h>

#include <thread>

int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    GraphicsBase graphics(800, 800);


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
                pos.makeMove(encodeMove(static_cast<Square>(square), static_cast<Square>(square +2),CASTLING));
                graphics.drawBoard(pos, {});
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_u){
                    pos.undoMove();
                    graphics.drawBoard(pos, {});
                }
            }
        }
    }

    graphics.close();
    return 0;
}