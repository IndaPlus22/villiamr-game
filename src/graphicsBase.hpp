#ifndef GRAPHICS_BASE
#define GRAPHICS_BASE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <filesystem>
#include <iostream>
#include "types.hpp"



class GraphicsBase {
public:
    GraphicsBase(SDL_Renderer* renderer, int width, int height);
    ~GraphicsBase();

    void drawBase();

private:
    SDL_Renderer* renderer;

    int ScreenWidth;
    int ScreenHeight;
    int squareSize;

    SDL_Texture* bg;
    SDL_Rect bg_rect;

    SDL_Rect piece_rect;

    SDL_Texture* PieceTextures[12];
};

#endif