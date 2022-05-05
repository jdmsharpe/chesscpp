#include "Game.h"

#include <ctype.h>
#include <fstream>
#include <sstream>

namespace {
const std::regex legalMove = std::regex("[a-hA-H][1-8]");
const std::regex delimiters = std::regex("[/ +]");
const std::regex numbers = std::regex("[1-8]");

const std::unordered_map<char, int> letterToIndex = {
    {'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5},
    {'g', 6}, {'h', 7}, {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3},
    {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7}};

const std::unordered_map<char, int> numberToIndex = {
    {'1', 0}, {'2', 1}, {'3', 2}, {'4', 3}, {'5', 4},
    {'6', 5}, {'7', 6}, {'8', 7}
};

const std::string k_fenFilename = "../chess/inc/load.fen";

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

BoardLayout Game::parseFen() const {
  std::string line = "";
  std::ifstream ifs;
  BoardLayout layout;

  ifs.open(k_fenFilename);
  if (ifs.is_open()) {
    while (std::getline(ifs, line)) {
      // This is honestly kind of insane... it's only two lines and it uses
      // multiple delimiters! Full credit to darune in
      // https://stackoverflow.com/questions/7621727/split-a-string-into-words-by-multiple-delimiters
      std::sregex_token_iterator first{line.begin(), line.end(), delimiters,
                                       -1},
          last;
      std::vector<std::string> tokens = {first, last};

      // Current board position storage
      // Offset for black - in FEN format, black comes first
      Position currentPos = {0, 7};

      // Additional variable to keep track of inner loop board position
      int k = 0;

      for (int i = 0; i < static_cast<int>(tokens.size()); ++i) {
        // First parse the board
        if (i < 8) {
          for (int j = 0; j < static_cast<int>(tokens[i].size()); ++j) {
            const char token = tokens[i][j];
            currentPos = {k, 7 - i};
            Color color;

            // Check if the character is a letter or number
            // If it's a letter, it's a piece, and color can be identified by case
            // If it's a number, it specifies how many spaces until the next piece
            if (isalpha(token)) {
                if (isupper(token)) {
                    color = Color::white;
                } else if (islower(token)) {
                    color = Color::black;
                } else {
                    // Error parsing FEN
                }
            } else if (isdigit(token)) {
                k += static_cast<int>(token);
            }

            if (token == 'p' || token == 'P') {
                layout.pawns.push_back({color, currentPos});
            }

            if (token == 'n' || token == 'N') {
                layout.knights.push_back({color, currentPos});
            }

            if (token == 'b' || token == 'B') {
                layout.bishops.push_back({color, currentPos});
            }

            if (token == 'r' || token == 'R') {
                layout.rooks.push_back({color, currentPos});
            }

            if (token == 'q' || token == 'Q') {
                layout.queens.push_back({color, currentPos});
            }

            if (token == 'k' || token == 'K') {
                layout.kings.push_back({color, currentPos});
            }

            ++k;

            if (j == static_cast<int>(tokens[i].size() - 1)) {
                k = 0;
            } 
          }
        }
      }
    }
    ifs.close();
  }

  return layout;
}
