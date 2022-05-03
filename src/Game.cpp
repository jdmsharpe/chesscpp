#include "Game.h"

namespace {
const std::regex legalMove = std::regex("[a-hA-H][1-8]");

const std::unordered_map<char, int> letterToIndex = {
    {'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5},
    {'g', 6}, {'h', 7}, {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3},
    {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7}};

const std::unordered_map<char, int> numberToIndex = {
    {'1', 0}, {'2', 1}, {'3', 2}, {'4', 3}, {'5', 4},
    {'6', 5}, {'7', 6}, {'8', 7}

};
} // namespace

void Game::outputPlayerTurn() const {
  std::string currentPlayer = m_whiteToMove ? "White" : "Black";
  std::cout << currentPlayer
            << " to move. Please enter your selection and desired position."
            << std::endl;
}

bool Game::parseMove(const std::pair<std::string, std::string> input,
                     std::pair<Position, Position> &output) const {
  // Sanity check
  if (input.first.length() != 2 || input.second.length() != 2 ||
      !std::regex_search(input.first, legalMove) ||
      !std::regex_search(input.second, legalMove)) {
    std::cout << "Error: inputs were invalid." << std::endl;
    explainMoveFormat();
    return false;
  }

  output.first = {letterToIndex.at(input.first[0]),
                  numberToIndex.at(input.first[1])};
  output.second = {letterToIndex.at(input.second[0]),
                   numberToIndex.at(input.second[1])};

  return true;
}

void Game::explainMoveFormat() const {
  std::cout << "Moves should be input in standard chessboard notation, i.e. "
            << std::endl
            << "treating horizontal squares as A through F, and vertical "
               "squares as 1 through 8. "
            << std::endl
            << "The first entry should be the position of the piece to move, "
            << std::endl
            << "and the second should be the desired position to move it to. "
            << std::endl
            << "As an example, to move White's E2 pawn to E4, you can type "
               "either 'e2 e4' or 'E2 E4'."
            << std::endl;
}
