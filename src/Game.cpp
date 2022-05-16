#include "Game.h"

#include <ctype.h>
#include <fstream>
#include <sstream>

namespace {
const std::regex k_legalMove = std::regex("[a-hA-H][1-8]");
const std::regex k_legalPromotion = std::regex("[nNbBrRqQ]");
const std::regex k_fenCastle = std::regex("[kK]*[qQ]*");
const std::regex k_delimiters = std::regex("[/ +]");
const std::regex k_numbers = std::regex("[1-8]");

const std::unordered_map<char, int> k_letterToIndex = {
    {'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5},
    {'g', 6}, {'h', 7}, {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3},
    {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7}};

const std::unordered_map<char, int> k_numberToIndex = {
    {'1', 0}, {'2', 1}, {'3', 2}, {'4', 3},
    {'5', 4}, {'6', 5}, {'7', 6}, {'8', 7}};

const std::string k_fenFilename = "../chess/inc/load.fen";

// Used for correctly parsing FEN strings
constexpr int k_whoseTurnIndex = 8;
constexpr int k_castlingIndex = 9;
constexpr int k_enPassantIndex = 10;
constexpr int k_halfMoveIndex = 11;
constexpr int k_turnIndex = 12;

} // namespace

void Game::whoWon() const {
  if (!m_winner.has_value()) {
    // Add draw functionality later
    return;
  }

  std::string winner = (m_winner == Color::white) ? "White" : "Black";
  std::cout << winner << " won the game!" << std::endl;
}

void Game::outputPlayerTurn() const {
  std::string currentPlayer = m_whiteToMove ? "White" : "Black";
  std::cout << currentPlayer
            << " to move. Please enter your selection and desired position."
            << std::endl;
}

bool Game::parseMove(const std::pair<std::string, std::string> &input,
                     std::pair<Position, Position> &output) const {
  // Sanity check
  if (input.first.length() != 2 || input.second.length() != 2 ||
      !std::regex_search(input.first, k_legalMove) ||
      !std::regex_search(input.second, k_legalMove)) {
    std::cout << "Error: inputs were invalid." << std::endl;
    outputMoveFormat();
    return false;
  }

  output.first = {k_letterToIndex.at(input.first[0]),
                  k_numberToIndex.at(input.first[1])};
  output.second = {k_letterToIndex.at(input.second[0]),
                   k_numberToIndex.at(input.second[1])};

  return true;
}

bool Game::parsePromotion(const std::string &input, PieceType &output) const {
  if (input.size() > 1 || !std::regex_search(input, k_legalPromotion)) {
    std::cout << "Error: promotion input was invalid." << std::endl;
    return false;
  }

  if (input[0] == 'n' || input[0] == 'N') {
    output = PieceType::knight;
  } else if (input[0] == 'b' || input[0] == 'B') {
    output = PieceType::bishop;
  } else if (input[0] == 'r' || input[0] == 'R') {
    output = PieceType::rook;
  } else if (input[0] == 'q' || input[0] == 'Q') {
    output = PieceType::queen;
  }

  return true;
}

void Game::outputMoveFormat() const {
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

void Game::outputPromotionRules() const {
  std::cout << "Please enter the letter of the piece you would like to promote "
            << std::endl
            << "to. For example, to promote to queen, enter Q, and to get a "
            << std::endl
            << "knight, enter N." << std::endl;
}

BoardLayout Game::parseFen() {
  std::string line = "";
  std::ifstream ifs;
  BoardLayout layout;

  ifs.open(k_fenFilename);
  if (ifs.is_open()) {
    while (std::getline(ifs, line)) {
      // This is honestly kind of insane... it's only two lines and it uses
      // multiple delimiters! Full credit to darune in
      // https://stackoverflow.com/questions/7621727/split-a-string-into-words-by-multiple-delimiters
      std::sregex_token_iterator first{line.begin(), line.end(), k_delimiters,
                                       -1},
          last;
      std::vector<std::string> tokens = {first, last};

      // Current board position storage
      // Offset for black - in FEN format, black comes first
      Position currentPos = {0, 7};

      for (int i = 0; i < static_cast<int>(tokens.size()); ++i) {
        // Additional variable to keep track of inner loop board position
        int k = 0;
        // First parse the board
        if (i < k_whoseTurnIndex) {
          for (int j = 0; j < static_cast<int>(tokens[i].size()); ++j) {
            const char token = tokens[i][j];
            currentPos = {k, 7 - i};
            Color color;

            // Check if the character is a letter or number
            // If it's a letter, it's a piece, and color derives from case
            // If it's a number, it specifies how many spaces until the
            // next piece
            if (isalpha(token)) {
              if (isupper(token)) {
                color = Color::white;
              } else if (islower(token)) {
                color = Color::black;
              }

              if (token == 'p' || token == 'P') {
                layout.pawns.push_back({color, currentPos});
              } else if (token == 'n' || token == 'N') {
                layout.knights.push_back({color, currentPos});
              } else if (token == 'b' || token == 'B') {
                layout.bishops.push_back({color, currentPos});
              } else if (token == 'r' || token == 'R') {
                layout.rooks.push_back({color, currentPos});
              } else if (token == 'q' || token == 'Q') {
                layout.queens.push_back({color, currentPos});
              } else if (token == 'k' || token == 'K') {
                layout.kings.push_back({color, currentPos});
              }

              ++k;

            } else if (isdigit(token)) {
              // The fact that static_casting to int doesn't work here
              // is quite cursed. I guess this is what C is like
              k += token - '0';
            }

            if (j == static_cast<int>(tokens[i].size() - 1)) {
              k = 0;
            }

            continue;
          }
        }

        if (i == k_whoseTurnIndex) {
          layout.whoseTurn = (tokens[i] == "w") ? Color::white : Color::black;
          setTurn(layout.whoseTurn);
          continue;
        }

        if (i == k_castlingIndex) {
          if (tokens[i] == "-") {
            // Nothing to record
            continue;
          } else {
            if (tokens[i].length() <= 4 &&
                std::regex_search(tokens[i], k_fenCastle)) {
              for (const auto &token : tokens[i]) {
                if (token == 'k') {
                  layout.castleStatus.set(k_blackKingsideIndex, true);
                } else if (token == 'K') {
                  layout.castleStatus.set(k_whiteKingsideIndex, true);
                } else if (token == 'q') {
                  layout.castleStatus.set(k_blackQueensideIndex, true);
                } else if (token == 'Q') {
                  layout.castleStatus.set(k_whiteQueensideIndex, true);
                }
              }
            }
          }
        }

        if (i == k_enPassantIndex) {
          if (tokens[i] == "-") {
            continue;
          } else {
            if (tokens[i].length() == 2 &&
                std::regex_search(tokens[i], k_legalMove)) {
              layout.enPassantTarget = {k_letterToIndex.at(tokens[i][0]),
                                        k_numberToIndex.at(tokens[i][1])};
            }
          }
        }

        if (i == k_halfMoveIndex) {
          size_t halfMove = 0;
          if (sscanf(tokens[i].c_str(), "%zu", &halfMove) == 1) {
            layout.halfMoveNum = halfMove;
            continue;
          }
        }

        if (i == k_turnIndex) {
          size_t turn = 0;
          if (sscanf(tokens[i].c_str(), "%zu", &turn) == 1) {
            layout.turnNum = turn;
            continue;
          }
        }
      }
    }
    ifs.close();
  }

  return layout;
}
