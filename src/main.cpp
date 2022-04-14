#include "Board.h"

int main() {
    std::cout << "Hello world!" << std::endl;
    Board b;

    std::cout << b.getPieceAt({0, 0})->isValidMove({0, 2}) << std::endl;
    b.getPieceAt({0, 0})->setPosition({0, 2});
    std::cout << b.getPieceAt({0, 0})->isValidMove({0, 3}) << std::endl;

    return 0;
}