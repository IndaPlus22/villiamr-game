#include "graphicsBase.hpp"

GraphicsBase::GraphicsBase(SDL_Renderer* renderer, int width, int height) {
    this->ScreenHeight = height;
    this->ScreenWidth = width;
    this->squareSize = width / 8;
    this->renderer = renderer;
    this->bg_rect = {0, 0, this->ScreenWidth, this->ScreenHeight};
    this->piece_rect = {125, 125, this->ScreenWidth / 8, this->ScreenHeight / 8};

    // GET FILE PATH TO ASSETS
    std::string current_path = std::filesystem::current_path().string();
    std::string projectRoot = "villiamr-game";
    size_t pos = current_path.find(projectRoot);
    std::string assetsPath = current_path.substr(0, pos + 14) + "assets/";

    // LOAD BACKGROUND
    std::cout << "[\33[1;32mLoading\33[0m] "  << "chessboard.png" << std::endl;
    this->bg = IMG_LoadTexture(renderer, (assetsPath + "chessboard.png").c_str());
    if(this->bg == nullptr) {
        SDL_Log("Unable to load background: %s", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    // LOAD PIECES
    std::string piecePaths[12] = {"Wpawn.png", "Wbishop.png", "Wknight.png", "Wrook.png", "Wqueen.png", "Wking.png", "Bpawn.png", "Bbishop.png", "Bknight.png", "Brook.png", "Bqueen.png", "Bking.png"};
    for (PieceType i = wPAWN; i < NO_PIECE; i++) {
        std::cout << "[\33[1;32mLoading\33[0m] " << piecePaths[i] << std::endl;
        this->PieceTextures[i] = IMG_LoadTexture(renderer, (assetsPath + piecePaths[i]).c_str());
        if(this->PieceTextures[i] == nullptr) {
            SDL_Log("[\33[1;31mUnable to load piece:\33[0m] %s", IMG_GetError());
            exit(EXIT_FAILURE);
        }
    }

   
}

GraphicsBase::~GraphicsBase() {
    SDL_DestroyTexture(this->bg);
    for (PieceType i = wPAWN; i < NO_PIECE; i++) {
        SDL_DestroyTexture(this->PieceTextures[i]);
    }
}

void GraphicsBase::drawPosition(Position position) { // TODO: THIS FUNCTION SHOULD TAKE A POSITION OBJECT AS ARGUMENT AND DRAW THE BOARD ACCORDING TO THE POSITION
    SDL_RenderCopy(this->renderer, this->bg, NULL, &this->bg_rect);
    for (PieceType i = wPAWN; i < NO_PIECE; i++) {
        for (int j = 0; j < 64; j++) {
            if (position.getBitboard(i) & (1ULL << j)) {
                this->piece_rect.x = (j % 8) * this->squareSize;
                this->piece_rect.y = (j / 8) * this->squareSize;
                SDL_RenderCopy(this->renderer, this->PieceTextures[i], NULL, &this->piece_rect);
            }
                  
        }
    }
}

void GraphicsBase::highlightSquare(int square) {
    SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255);
    SDL_Rect rect = {square % 8 * this->squareSize, square / 8 * this->squareSize, this->squareSize, this->squareSize};
    SDL_RenderDrawRect(this->renderer, &rect);
}