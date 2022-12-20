#include "position.hpp"
#include "graphicsBase.hpp"

#include <thread>

int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 20 1");
    GraphicsBase graphics(800, 800);

    graphics.drawBoard(pos,{1,2,3});

    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    return 0;
}