#include "position.hpp"
#include "graphicsBase.hpp"
#include "movegen.hpp"
#include "Engine.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

int hojballe = 3195901860;

Bitboard perft(int depth, Position &position,GraphicsBase &graphics){
    Bitboard nodes = 0, nMoves = 0;
    std::vector<move> moves = generateLegalMoves(position);
    nMoves = moves.size();

    if (depth == 1){
        return nMoves;
    }

    for (move m : moves){
        position.makeMove(m);
        //graphics.drawBoard(position, {});
        nodes += perft(depth - 1, position,graphics);
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
    //GraphicsBase graphics(800, 800);
    Engine engine(WHITE, 7);
    std::thread t;

    const std::string squares[64] = {"a8","b8","c8","d8","e8","f8","g8","h8",
                                     "a7","b7","c7","d7","e7","f7","g7","h7",
                                     "a6","b6","c6","d6","e6","f6","g6","h6",
                                     "a5","b5","c5","d5","e5","f5","g5","h5",
                                     "a4","b4","c4","d4","e4","f4","g4","h4",
                                     "a3","b3","c3","d3","e3","f3","g3","h3",
                                     "a2","b2","c2","d2","e2","f2","g2","h2",
                                     "a1","b1","c1","d1","e1","f1","g1","h1"};

    const std::string movetypes[] = {"QUIET", "DOUBLE_PAWN_PUSH","CASTLING","EN_PASSANT","CAPTURE","PROMOTON","PROMOTION_CAPTURE"};

    t = std::thread(&Engine::findBestMove, &engine, pos);
    bool uci = true;
    while(uci){
        std::string input;
        getline(std::cin, input);
        std::string token = input.substr(0, input.find(" "));
        if(token == "uci")
            std::cout << "uciok" << std::endl;
        if (token == "isready")
            std::cout << "readyok" << std::endl;
        else if (token == "go"){
            if(t.joinable())
                t.join();
            move k = engine.getEngineMove();
            pos.makeMove(k);
            std::cout << "info score cp " << engine.evaluatePosition(pos) << std::endl;
            bool promotion = false;
            if (getMoveType(k) == PROMOTON || getMoveType(k) == PROMOTION_CAPTURE)
                promotion = true;
            std::cout << "bestmove " << squares[getFromSquare(k)] << squares[getToSquare(k)] << (promotion ? "Q" : "") << std::endl;
        }
        if (token == "quit" || token == "stop"){
            t.join();
            uci = false;
        }
        if (token == "position"){
            if(t.joinable())
                t.join();
            std::vector<move> moves = generateLegalMoves(pos);
            input = input.substr(input.size() - 4, input.size());
            std::string startsq = input.substr(0,2);
            std::string endsq = input.substr(2,2);
            int start;
            int to;
            for (int i = 0; i < 64; i++){
                if (squares[i] == startsq)
                    start = i;
                if (squares[i] == endsq)
                    to = i;
            }
            for (move m : moves){
                if (getFromSquare(m) == start && getToSquare(m) == to){
                    pos.makeMove(m);
                    break;
                }
            }
            t = std::thread(&Engine::findBestMove, &engine, pos);
        }
            
    }





    //std::cout << "\rPerft: " << perft(8, pos,graphics) << std::endl;
    //graphics.drawBoard(pos, {});
    // std::vector<int> highlightedSquares;
    // std::vector<move> moves = generateLegalMoves(pos);
    // bool running = true;
    // bool engineTurn = false;
    // while (running){
    //     SDL_Event event;
    //     while (SDL_PollEvent(&event)){
    //         if (event.type == SDL_QUIT){
    //             running = false;
    //             break;
    //         }
    //         if(pos.getCheckmate() || pos.getGameIsOver()){
    //             running = false;
    //             if(pos.getCheckmate()){
    //                 std::cout << "Checkmate winner is: " << (pos.getSideToMove() == WHITE ? "BLACK" : "WHITE") << std::endl;
    //             }
    //             else{
    //                 std::cout << "Stalemate" << std::endl;
    //             }
    //         }
    //         // if(pos.getSideToMove() == engine.getPlayerColor() && !engineTurn){
    //         //     threadFunction(pos, engine, graphics, engineTurn);
    //         // }
    //         if (event.type == SDL_MOUSEBUTTONDOWN && !engineTurn){
    //             moves = generateLegalMoves(pos);
    //             int x,y;
    //             SDL_GetMouseState(&x, &y);
    //             highlightedSquares.push_back((x/100) + 8*(y/100));
    //             if(highlightedSquares.size() == 1){
    //                 for(move m : moves){
    //                     if(getFromSquare(m) == highlightedSquares[0]){
    //                         highlightedSquares.push_back(getToSquare(m));
    //                         //graphics.drawBoard(pos, highlightedSquares);
    //                     }
    //                 }
    //             }else {
    //                 for(move m : moves){
    //                     if(getFromSquare(m) == highlightedSquares[0] && getToSquare(m) == highlightedSquares.back()){
    //                         pos.makeMove(m);
    //                         moves = generateLegalMoves(pos);
    //                     }
    //                 }
    //                 highlightedSquares.clear();
    //                 //graphics.drawBoard(pos, {});
    //             }
                
    //         }
    //         if(event.type == SDL_KEYDOWN && !engineTurn){
    //             if(event.key.keysym.sym == SDLK_u){
    //                 pos.undoMove();
    //                 moves = generateLegalMoves(pos);
    //                 //graphics.drawBoard(pos, {});
    //             }
    //         }
    //     }
    //     //if(highlightedSquares.size() == 0){
    //     //    graphics.drawBoard(pos, {});
    //     //}
    // }

    //graphics.close();
    return 0;
}