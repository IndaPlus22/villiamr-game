#include "position.hpp"
#include "graphicsBase.hpp"
#include "movegen.hpp"
#include "Engine.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include <fstream>


int engineDepth = 10;

void uciLoop();
void graphicsLoop(bool whiteEngine, bool blackEngine);

int main(int argc, char *argv[]){
    bool uci = false;

    if (argc == 1){
        uciLoop();
    }else{
        // Input format
            // ./chess <White> <Black> <Depth>

        if(argc != 4){
            std::cout << "Invalid input format" << std::endl;
            std::cout << "\tInput format: ./chess <White> <Black> <Depth>" << std::endl;
            std::cout << "\t\tWhite: human, engine" << std::endl;
            std::cout << "\t\tBlack: human, engine" << std::endl;
            std::cout << "\t\tRecomended depth: 1-10" << std::endl;
            return 0;
        }
        bool whiteEngine = false;
        std::string white = argv[1];
        if (white == "engine"){
            whiteEngine = true;
        }
        bool blackEngine = false;
        std::string black = argv[2];
        if (black == "engine"){
            blackEngine = true;
        }
        engineDepth = std::stoi(argv[3]);
        
        graphicsLoop(whiteEngine, blackEngine);
    }


    return 0;
}
























//**********************************************************************************************************************
//****************************************************PROGRAM MAIN LOOPS************************************************
//**********************************************************************************************************************


void graphicsLoop(bool whiteEngine, bool blackEngine){
    GraphicsBase graphics(800, 800);
    Position pos = Position();
    pos.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Engine engine(engineDepth);
    Engine engine2(engineDepth);

    //std::cout << "\rPerft: " << perft(8, pos,graphics) << std::endl;
    graphics.drawBoard(pos, {});
    std::vector<int> highlightedSquares;
    std::vector<move> moves = generateLegalMoves(pos);
    bool running = true;
    bool engineTurn = true;
    Color side = pos.getSideToMove();
    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                running = false;
                break;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && ((pos.getSideToMove() == WHITE && whiteEngine == false) || (pos.getSideToMove() == BLACK && blackEngine == false))){
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
                            graphics.drawBoard(pos, {});
                        }
                    }
                    highlightedSquares.clear();
                    graphics.drawBoard(pos, {});
                }

            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_u){
                    pos.undoMove();
                    moves = generateLegalMoves(pos);
                    graphics.drawBoard(pos, {});
                }
            }
        }
        if(side != pos.getSideToMove()){
            engineTurn = true;
            side = pos.getSideToMove();
        }

        generateLegalMoves(pos);
        if(pos.getCheckmate() || pos.getStalemate()){
                pos.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                graphics.drawBoard(pos, {});
                engine.clearTT();
                engine2.clearTT();
        }
        else if(pos.getSideToMove() == BLACK && blackEngine && engineTurn){
                std::cout << "Engine Black" << std::endl;
                //engine.findBestMove(pos);
                std::thread t(&Engine::findBestMove, &engine, std::ref(pos));
                t.detach();
                engineTurn = false;
        }
        else if(pos.getSideToMove() == WHITE && whiteEngine && engineTurn){
                std::cout << "Engine White" << std::endl;
                //engine2.findBestMove(pos);
                std::thread t(&Engine::findBestMove, &engine2, std::ref(pos));
                t.detach();
                engineTurn = false;
        }
        graphics.drawBoard(pos, highlightedSquares);
        //side = pos.getSideToMove();
    }

    graphics.close();
}




void uciPosition(std::string input, Position &pos);

void uciLoop()
{
    std::string input;
    Position pos = Position();
    Engine engine(engineDepth);

    std::thread t;

    bool uci = true;
    while (uci)
    {
        std::string input;
        getline(std::cin, input);
        std::string token = input.substr(0, input.find(" "));

        // UCI PROOTOCOL DEFINES THE FOLLOWING COMMANDS
        // uci - tells the engine to use the uci protocol for input/output
        // isready - tells the engine to check if it is ready to accept a new game
        // position - sets up the position described in fenstring on the internal board or "startpos" with the moves from the movelist
        // go - tells the engine to start calculating on the current position set up with the "position" command
        // stop - tells the engine to stop calculating as soon as possible
        // quit - tells the engine to quit as soon as possible

        if (token == "uci")
            std::cout << "uciok" << std::endl;
        else if (token == "isready")
            std::cout << "readyok" << std::endl;

        else if (token == "position")
        {
            uciPosition(input, pos);
        }

        else if (token == "go")
        {
            engine.findBestMove(pos);
        }

        else if (token == "stop" || token == "quit")
        {
            uci = false;
        }
    }
}

void uciPosition(std::string input, Position &pos){
    
    std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string tricky = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    input = input.substr(input.find(" ") + 1);
    std::string option = input.substr(0, input.find(" "));

    // SET UP POSITION
    if (option == "startpos")
    {
        pos.setFen(startpos);
    }
    else if (option == "tricky")
    {
        pos.setFen(tricky);
    }
    else if (option == "fen")
    {
        input = input.substr(input.find("fen") + 4);
        std::string fen = input.substr(0, input.find("moves"));
        pos.setFen(fen);
    }

    //std::cout << "Original Hash: " << pos.getHash() << std::endl;

    // PARSE MOVES
    if (input.find("moves") != std::string::npos)
    {
        input = input.substr(input.find("moves") + 6);
        while (input.size() >= 4)
        {
            std::string UCImove = input.substr(0, input.find(" "));

            int from = (UCImove[0] - 'a') - 8 * (UCImove[1] - '8');
            int to = (UCImove[2] - 'a') - 8 * (UCImove[3] - '8');


            for (move m : generateLegalMoves(pos))
            {
                if (getFromSquare(m) == from && getToSquare(m) == to)
                {
                    pos.makeMove(m);
                    break;
                }
            }
            if (input.size() <= 5)
                break;
            input = input.substr(input.find(" ") + 1);
        }
    }
}