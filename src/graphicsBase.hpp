#ifndef GRAPHICS_BASE_HPP
#define GRAPHICS_BASE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <filesystem>
#include "position.hpp"

class GraphicsBase {
public:
    GraphicsBase(int width, int height);
    ~GraphicsBase() = default;

    void drawBoard(Position position, std::vector<int> highlightedSquares);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    int ScreenWidth;
    int ScreenHeight;
    int squareSize;

    SDL_Texture* bg;
    SDL_Rect bg_rect;

    SDL_Rect piece_rect;

    SDL_Texture* PieceTextures[12];

    void drawPosition(Position position);
    void highlightSquare(int square);
};


#endif