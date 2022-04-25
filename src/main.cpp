#include "Board.h"
#include "Game.h"

int main(int argc, char **argv)
{
    Board board;
    Game game;

    // Initialize containers for player inputs
    std::pair<std::string, std::string> input;
    std::pair<Position, Position> output;

    while (game.isInProgress())
    {
        board.display();
        game.outputPlayerTurn();
        std::cin >> input.first >> input.second;

        game.parseMove(input, output);

        board.checkMove(game.whoseTurnIsIt(), output.first, output.second);
        std::cout << board.isKingInCheck(game.whoseTurnIsIt());
        // When move is complete, turn is over
        game.switchPlayers();

        // // Clear terminal (should investigate a better way to do this)
        // std::cout << "\033[H\033[2J\033[3J";
    }

    return 0;
}
