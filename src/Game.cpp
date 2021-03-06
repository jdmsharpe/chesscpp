#include "Game.h"

#include <ctype.h>
#include <filesystem>
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

// Used for correctly parsing FEN strings
constexpr int k_whoseTurnIndex = 8;
constexpr int k_castlingIndex = 9;
constexpr int k_enPassantIndex = 10;
constexpr int k_halfMoveIndex = 11;
constexpr int k_turnIndex = 12;

constexpr int k_whiteEnPassantRow = 3;

} // namespace

void Game::reset() {
  m_inProgress = true;
  m_whiteToMove = true;
  m_winner.reset();
  m_displayWinnerOnceFlag = true;

  m_fiftyMoveRuleNum = 0;
  m_halfMoveNum = 0;
  m_turnNum = 1;
}

void Game::whoWon() const {
  // Only print one time
  if (m_displayWinnerOnceFlag) {
    m_displayWinnerOnceFlag = false;
    // Draw case
    if (!m_winner.has_value()) {
      std::cout << "The game was a draw!" << std::endl;
      return;
    }

    std::string winner = (m_winner == Color::white) ? "White" : "Black";
    std::cout << winner << " won the game!" << std::endl;
  }
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

void Game::outputKingInCheck() const {
  std::string currentPlayer = m_whiteToMove ? "White" : "Black";
  std::cout << currentPlayer << "'s king is in check!" << std::endl;
}

LumpedBoardAndGameState Game::parseFen(const std::string filename,
                                       const int lineNumber) {
  std::string line = "";
  std::ifstream ifs;
  LumpedBoardAndGameState state;
  int localLineNumber = 0;

  ifs.open(filename);
  if (ifs.is_open()) {
    while (std::getline(ifs, line)) {
      if (localLineNumber == lineNumber) {
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
                  state.pawns.emplace_back(color, currentPos);
                } else if (token == 'n' || token == 'N') {
                  state.knights.emplace_back(color, currentPos);
                } else if (token == 'b' || token == 'B') {
                  state.bishops.emplace_back(color, currentPos);
                } else if (token == 'r' || token == 'R') {
                  state.rooks.emplace_back(color, currentPos);
                } else if (token == 'q' || token == 'Q') {
                  state.queens.emplace_back(color, currentPos);
                } else if (token == 'k' || token == 'K') {
                  state.kings.emplace_back(color, currentPos);
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
            state.whoseTurn = (tokens[i] == "w") ? Color::white : Color::black;
            setTurn(state.whoseTurn);
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
                    state.castleStatus.set(k_blackKingsideIndex, true);
                  } else if (token == 'K') {
                    state.castleStatus.set(k_whiteKingsideIndex, true);
                  } else if (token == 'q') {
                    state.castleStatus.set(k_blackQueensideIndex, true);
                  } else if (token == 'Q') {
                    state.castleStatus.set(k_whiteQueensideIndex, true);
                  }
                }
              }
            }
          }

          if (i == k_enPassantIndex) {
            if (tokens[i] == "-") {
              state.enPassantStatus = std::nullopt;
              continue;
            } else {
              if (tokens[i].length() == 2 &&
                  std::regex_search(tokens[i], k_legalMove)) {
                Color color = (tokens[i][1] == k_whiteEnPassantRow)
                                  ? Color::white
                                  : Color::black;
                state.enPassantStatus = {color,
                                         {k_letterToIndex.at(tokens[i][0]),
                                          k_numberToIndex.at(tokens[i][1])}};
                continue;
              }
            }
          }

          if (i == k_halfMoveIndex) {
            size_t halfMove = 0;
            if (sscanf(tokens[i].c_str(), "%zu", &halfMove) == 1) {
              state.halfMoveNum = halfMove;
              continue;
            }
          }

          if (i == k_turnIndex) {
            size_t turn = 0;
            if (sscanf(tokens[i].c_str(), "%zu", &turn) == 1) {
              state.turnNum = turn;
              continue;
            }
          }
        }
      }
      // Increment line number
      ++localLineNumber;
    }
  }
  ifs.close();

  return state;
}

void Game::writeToFen(const std::string filename,
                      const LumpedBoardAndGameState &boardAndGameState) {
  std::string line = "";
  // Append to existing content
  std::ofstream ofs(filename, std::ios::app);

  std::vector<PiecesForFenStorage> piecesForOutput = {};
  Position positionToAdd = {};
  int emptySpaceCounter = 0;

  auto positionMatch = [&positionToAdd](const PiecesForFenStorage &piece) {
    if (positionToAdd == piece.position) {
      return true;
    }

    return false;
  };

  if (ofs.is_open()) {
    for (const auto &pawn : boardAndGameState.pawns) {
      piecesForOutput.emplace_back(
          pawn.first, PieceType::pawn,
          Position({pawn.second.first, pawn.second.second}));
    }
    for (const auto &knight : boardAndGameState.knights) {
      piecesForOutput.emplace_back(
          knight.first, PieceType::knight,
          Position({knight.second.first, knight.second.second}));
    }
    for (const auto &bishop : boardAndGameState.bishops) {
      piecesForOutput.emplace_back(
          bishop.first, PieceType::bishop,
          Position({bishop.second.first, bishop.second.second}));
    }
    for (const auto &rook : boardAndGameState.rooks) {
      piecesForOutput.emplace_back(
          rook.first, PieceType::rook,
          Position({rook.second.first, rook.second.second}));
    }
    for (const auto &queen : boardAndGameState.queens) {
      piecesForOutput.emplace_back(
          queen.first, PieceType::queen,
          Position({queen.second.first, queen.second.second}));
    }
    for (const auto &king : boardAndGameState.kings) {
      piecesForOutput.emplace_back(
          king.first, PieceType::king,
          Position({king.second.first, king.second.second}));
    }

    // Go through every square on board
    for (int i = 0; i < 8; ++i) {
      emptySpaceCounter = 0;
      for (int j = 0; j < 8; ++j) {
        positionToAdd = {j, 7 - i};

        auto it = std::find_if(piecesForOutput.cbegin(), piecesForOutput.cend(),
                               positionMatch);

        if (it != piecesForOutput.cend()) {
          if (emptySpaceCounter >= 1) {
            line.push_back('0' + emptySpaceCounter);
          }

          emptySpaceCounter = -1;

          if (it->type == PieceType::pawn) {
            line.push_back((it->color == Color::white) ? 'P' : 'p');
          } else if (it->type == PieceType::knight) {
            line.push_back((it->color == Color::white) ? 'N' : 'n');
          } else if (it->type == PieceType::bishop) {
            line.push_back((it->color == Color::white) ? 'B' : 'b');
          } else if (it->type == PieceType::rook) {
            line.push_back((it->color == Color::white) ? 'R' : 'r');
          } else if (it->type == PieceType::queen) {
            line.push_back((it->color == Color::white) ? 'Q' : 'q');
          } else if (it->type == PieceType::king) {
            line.push_back((it->color == Color::white) ? 'K' : 'k');
          } else {
            // Type ???
            continue;
          }
        }

        ++emptySpaceCounter;

        // if ()

        if (j == 7) {
          if (emptySpaceCounter >= 1) {
            line.push_back('0' + emptySpaceCounter);
          }
        }
      }
      if (i != 7) {
        line.push_back('/');
      }
    }

    line.push_back(' ');
    line.push_back((boardAndGameState.whoseTurn == Color::white) ? 'w' : 'b');

    line.push_back(' ');
    if (boardAndGameState.castleStatus.any()) {
      if (boardAndGameState.castleStatus[k_whiteKingsideIndex]) {
        line.push_back('K');
      }
      if (boardAndGameState.castleStatus[k_whiteQueensideIndex]) {
        line.push_back('Q');
      }
      if (boardAndGameState.castleStatus[k_blackKingsideIndex]) {
        line.push_back('k');
      }
      if (boardAndGameState.castleStatus[k_blackQueensideIndex]) {
        line.push_back('q');
      }
    } else {
      line.push_back('-');
    }

    line.push_back(' ');
    if (boardAndGameState.enPassantStatus.has_value()) {
      for (auto it = k_letterToIndex.begin(); it != k_letterToIndex.end();
           ++it) {
        if (it->second ==
            boardAndGameState.enPassantStatus.value().second.first) {
          if (std::islower(it->first)) {
            line.push_back(it->first);
          }
        }
      }
      for (auto it = k_numberToIndex.begin(); it != k_numberToIndex.end();
           ++it) {
        if (it->second ==
            boardAndGameState.enPassantStatus.value().second.second) {
          line.push_back(it->first);
        }
      }
    } else {
      line.push_back('-');
    }

    line.push_back(' ');
    line.push_back('0' + static_cast<int>(boardAndGameState.turnNum));
    line.push_back(' ');
    line.push_back('0' + static_cast<int>(boardAndGameState.halfMoveNum));
    line.push_back('\n');

    ofs << line;
  }

  ofs.close();
}