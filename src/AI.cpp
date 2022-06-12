#include "AI.h"
#include "Board.h"

#include <iterator>
#include <random>

namespace {

constexpr int k_minimaxDepth = 3;

constexpr int k_pawnValue = 100;
constexpr int k_knightValue = 300;
constexpr int k_bishopValue = 300;
constexpr int k_rookValue = 500;
constexpr int k_queenValue = 900;
constexpr int k_kingValue = 9000;

constexpr int k_maxSquareIndex = 7;

// Credits to https://www.chessprogramming.org/Simplified_Evaluation_Function
using EvalTable = int[k_totalSquares / 8][k_totalSquares / 8];
// Made last row 90 as that made more sense to me (guaranteed queen)
constexpr EvalTable k_pawnEvalTable = {
    {0,  0,  0,   0,   0,   0,   0,  0},
    {5,  10, 10,  -20, -20, 10,  10, 5},
    {5,  -5, -10, 0,   0,  -10, -5,  5},
    {0,  0,  0,   20,   20,  0,  0,  0},
    {5,  5,  10,  25,  25,  10,  5,  5},
    {10, 10, 20,  30,  30,  20,  10, 10},
    {50, 50, 50,  50,  50,  50,  50, 50},
    {90, 90, 90,  90,  90,  90,  90, 90}
};

constexpr EvalTable k_knightEvalTable = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20, 0,   5,   5,   0,   -20, -40},
    {-30, 5,   10,  15,  15,  10,  5,   -30},
    {-30, 0,   15,  20,  20,  15,  0,   -30},
    {-30, 5,   15,  20,  20,  15,  5,   -30},
    {-30, 0,   10,  15,  15,  10,  0,   -30},
    {-40, -20, 0,   0,   0,   0,   -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}
};

constexpr EvalTable k_bishopEvalTable = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10, 5,   0,   0,   0,   0,   5,   -10},
    {-10, 10,  10,  10,  10,  10,  10,  -10},
    {-10, 0,   10,  10,  10,  10,  0,   -10},
    {-10, 5,   5,   10,  10,  5,   5,   -10},
    {-10, 0,   5,   10,  10,  5,   0,   -10},
    {-10, 0,   0,   0,   0,   0,   0,   -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}
};

constexpr EvalTable k_rookEvalTable = {
    {0,  0,  0,  5,  5,  0,  0,  0},
    {-5, 0,  0,  0,  0,  0,  0,  -5},
    {-5, 0,  0,  0,  0,  0,  0,  -5},
    {-5, 0,  0,  0,  0,  0,  0,  -5},
    {-5, 0,  0,  0,  0,  0,  0,  -5},
    {-5, 0,  0,  0,  0,  0,  0,  -5},
    {5,  10, 10, 10, 10, 10, 10, 5},
    {0,  0,  0,  0,  0,  0,  0,  0}
};

// Changed this one a bit from the original
// to make it more symmetrical
constexpr EvalTable k_queenEvalTable = {
    {-20, -10, -10, -5, -5, -10, -10, -20},
    {-10, 0,   0,   0,  0,  0,   0,   -10},
    {-10, 0,   5,   5,  5,  5,   0,   -10},
    {-5,  0,   5,   5,  5,  5,   0,   -5},
    {-5,  0,   5,   5,  5,  5,   0,   -5},
    {-10, 0,   5,   5,  5,  5,   0,   -10},
    {-10, 0,   0,   0,  0,  0,   0,   -10},
    {-20, -10, -10, -5, -5, -10, -10, -20}
};

constexpr EvalTable k_kingEvalTable = {
    {20,  30,  10,  0,   0,   10,  30,  20},
    {20,  20,  0,   0,   0,   0,   20,  20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30}
};

// Credits to first answer:
// https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
template <typename it, typename RandomGenerator>
it selectRandomly(it start, it end, RandomGenerator &g) {
  std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
  std::advance(start, dis(g));
  return start;
}

template <typename it> it selectRandomly(it start, it end) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return selectRandomly(start, end, gen);
}

Color getOtherColor(Color color) {
  return (color == Color::white) ? Color::black : Color::white;
}

auto addToAdvantage = [](const PieceContainer &container, int pieceValue,
                         const EvalTable &evalTable) {
  int advantage = 0;
  for (size_t i = 0; i < container.size(); ++i) {
    const auto &piece = container[i];
    if (piece.second.first < 0 || piece.second.second < 0) {
      // Piece has been removed
      continue;
    }

    // Add raw piece value
    advantage += (piece.first == Color::white) ? pieceValue : -pieceValue;

    // Add evaluation table index for piece position
    // Evaluation tables are structured for white, so flip the table
    // vertically for black
    advantage += (piece.first == Color::white)
                     ? evalTable[piece.second.second][piece.second.first]
                     : -evalTable[k_maxSquareIndex - piece.second.second]
                                 [piece.second.first];
  }

  return advantage;
};

} // namespace

void AI::reset() {
  m_color = Color::black;
}

int AI::getAdvantage() {
  const auto &currentState = m_board.getBoardAndGameState(m_color.value());

  int advantage = 0;

  advantage += addToAdvantage(currentState.pawns, k_pawnValue, k_pawnEvalTable);
  advantage +=
      addToAdvantage(currentState.knights, k_knightValue, k_knightEvalTable);
  advantage +=
      addToAdvantage(currentState.bishops, k_bishopValue, k_bishopEvalTable);
  advantage += addToAdvantage(currentState.rooks, k_rookValue, k_rookEvalTable);
  advantage +=
      addToAdvantage(currentState.queens, k_queenValue, k_queenEvalTable);
  advantage += addToAdvantage(currentState.kings, k_kingValue, k_kingEvalTable);

  return advantage;
}

std::pair<Position, Position> AI::getRandomMove() {
  const auto &moves = m_board.getValidMovesFor(m_color.value());
  auto result = selectRandomly(moves.cbegin(), moves.cend());
  return {result->start, result->end};
}

std::pair<Position, Position> AI::minimaxRoot(Color max) {
  std::pair<Position, Position> bestMove;
  int bestAdvantage = -9999;
  const auto &startingMoves = m_board.getValidMovesFor(max);

  for (size_t i = 0; i < startingMoves.size(); ++i) {
    const auto &moveToMake = startingMoves[i];
    m_board.testMove(moveToMake.start, moveToMake.end, k_minimaxDepth);
    int advantage =
        minimax(getOtherColor(max), k_minimaxDepth - 1, -10000, 10000);
    m_board.undoMove(moveToMake.start, moveToMake.end, k_minimaxDepth);

    if (advantage >= bestAdvantage) {
      bestAdvantage = advantage;
      std::cout << advantage << std::endl;
      bestMove = std::make_pair(startingMoves[i].start, startingMoves[i].end);
    }
  }

  std::cout << "The best move advantage was: " << bestAdvantage << std::endl;

  return bestMove;
}

int AI::minimax(Color color, int depth, int alpha, int beta) {
  if (depth == 0) {
    int multiplier = (color == Color::white) ? -1 : 1;
    return getAdvantage() * multiplier;
  }

  m_board.refreshValidMoves();
  const auto &moves = m_board.getValidMovesFor(color);
  int bestAdvantage = 0;

  if (color == m_color.value()) {
    if (moves.size() == 0) {
      if (m_board.isKingInCheck(color)) {
        return -9999;
      }

      return 0;
    }

    bestAdvantage = -9999;

    for (size_t i = 0; i < moves.size(); ++i) {
      const auto &moveToMake = moves[i];
      m_board.testMove(moveToMake.start, moveToMake.end, depth);
      bestAdvantage = std::max(
          bestAdvantage, minimax(getOtherColor(color), depth - 1, alpha, beta));
      m_board.undoMove(moveToMake.start, moveToMake.end, depth);
      alpha = std::max(alpha, bestAdvantage);
      if (beta <= alpha) {
        return bestAdvantage;
      }
    }
  } else {
    if (moves.size() == 0) {
      if (m_board.isKingInCheck(getOtherColor(color))) {
        return 9999;
      }

      return 0;
    }

    bestAdvantage = 9999;

    for (size_t i = 0; i < moves.size(); ++i) {
      const auto &moveToMake = moves[i];
      m_board.testMove(moveToMake.start, moveToMake.end, depth);
      bestAdvantage = std::min(
          bestAdvantage, minimax(getOtherColor(color), depth - 1, alpha, beta));
      m_board.undoMove(moveToMake.start, moveToMake.end, depth);
      beta = std::min(beta, bestAdvantage);
      if (beta <= alpha) {
        return bestAdvantage;
      }
    }
  }

  return bestAdvantage;
}
