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
        std::cout << board.isKingInCheck(game.whoseTurnIsIt()) << std::endl;
        board.display();
        game.outputPlayerTurn();
        std::cin >> input.first >> input.second;

        game.parseMove(input, output);

        if (board.isValidMove(game.whoseTurnIsIt(), output.first, output.second))
        {
            board.movePiece(output.first, output.second);

            // When move is complete, turn is over
            game.switchPlayers();
        }

        // Clear terminal (should investigate a better way to do this)
        std::cout << "\033[H\033[2J\033[3J";
    }

    return 0;
}
