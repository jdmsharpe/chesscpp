#include "Game.h"

void Game::outputPlayerTurn() const
{
    std::string currentPlayer = m_whiteToMove ? "White" : "Black";
    std::cout << currentPlayer << " to move. Please enter your selection and desired position." << std::endl;
}

bool Game::parseMove(const std::pair<std::string, std::string> input, std::pair<Position, Position> &output) const
{
    // Sanity check
    if (input.first.length() != 2 || input.second.length() != 2) // || !std::isalnum(selectedPiece) || !std::isalnum(desiredPosition)
    {
        std::cout << "Error: inputs were invalid." << std::endl;
        explainMoveFormat();
        return false;
    }

    return true;
}

void Game::explainMoveFormat() const
{
    std::cout << "Moves should be input in standard chessboard notation, i.e. " << std::endl
              << "treating horizontal squares as A through F, and vertical squares as 1 through 8. " << std::endl
              << "The first entry should be the position of the piece to move, " << std::endl
              << "and the second should be the desired position to move it to. " << std::endl
              << "As an example, to move White's E2 pawn to E4, you can type either 'e2 e4' or 'E2 E4'." << std::endl;
}
