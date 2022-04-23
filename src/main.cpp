#include "Board.h"

int main() {
    std::cout << "Hello world!" << std::endl;
    Board b;

    b.display();
    b.checkMove({2, 0}, {3, 1});
    b.display();

    return 0;
}
