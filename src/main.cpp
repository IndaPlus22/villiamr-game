#include "position.hpp"
#include "graphicsBase.hpp"
#include "movegen.hpp"
#include "Engine.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

int hojballe = 0;

Bitboard perft(int depth, Position &position){
    if (depth == 0){
        return 1;
    }

    Bitboard nodes = 0;
    std::vector<move> moves = generateLegalMoves(position);
    if(position.getCheckmate()){
        hojballe++;
    }
    for (move m : moves){
        position.makeMove(m);
        nodes += perft(depth - 1, position);
        position.undoMove();
    }
    return nodes;
}

void threadFunction(Position &pos, Engine engine, GraphicsBase &graphics, bool &grabTurn){
    grabTurn = true;
    engine.findBestMove(pos);
    move k = engine.getEngineMove();
    std::cout << getFromSquare(k) << " " << getToSquare(k) << std::endl;
    pos.makeMove(k);
    graphics.drawBoard(pos, {});
    grabTurn = false;
}

int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    GraphicsBase graphics(800, 800);
    Engine engine(BLACK, 7);


    std::cout << "Perft: " << perft(5, pos) << std::endl;

    std::vector<int> highlightedSquares;
    std::vector<move> moves = generateLegalMoves(pos);
    bool running = true;
    bool engineTurn = false;
    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                running = false;
                break;
            }
            if(pos.getCheckmate() || pos.getGameIsOver()){
                running = false;
                if(pos.getCheckmate()){
                    std::cout << "Checkmate winner is: " << (pos.getSideToMove() == WHITE ? "BLACK" : "WHITE") << std::endl;
                }
                else{
                    std::cout << "Stalemate" << std::endl;
                }
            }
            if(pos.getSideToMove() == engine.getPlayerColor() && !engineTurn){
                threadFunction(pos, engine, graphics, engineTurn);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && !engineTurn){
                moves = generateLegalMoves(pos);
                int x,y;
                SDL_GetMouseState(&x, &y);
                highlightedSquares.push_back((x/100) + 8*(y/100));
                if(highlightedSquares.size() == 1){
                    for(move m : moves){
                        if(getFromSquare(m) == highlightedSquares[0]){
                            highlightedSquares.push_back(getToSquare(m));
                            graphics.drawBoard(pos, highlightedSquares);
                        }
                    }
                }else {
                    for(move m : moves){
                        if(getFromSquare(m) == highlightedSquares[0] && getToSquare(m) == highlightedSquares.back()){
                            pos.makeMove(m);
                            moves = generateLegalMoves(pos);
                        }
                    }
                    highlightedSquares.clear();
                    graphics.drawBoard(pos, {});
                }
                
            }
            if(event.type == SDL_KEYDOWN && !engineTurn){
                if(event.key.keysym.sym == SDLK_u){
                    pos.undoMove();
                    moves = generateLegalMoves(pos);
                    graphics.drawBoard(pos, {});
                }
            }
        }
        //if(highlightedSquares.size() == 0){
        //    graphics.drawBoard(pos, {});
        //}
    }

    graphics.close();
    return 0;
}