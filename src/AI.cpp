#include "AI.h"
#include "Board.h"

#include <iterator>
#include <random>

namespace {

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

constexpr int k_pawnValue = 10;
constexpr int k_knightValue = 30;
constexpr int k_bishopValue = 30;
constexpr int k_rookValue = 50;
constexpr int k_queenValue = 90;
constexpr int k_kingValue = 900;

} // namespace

void AI::reset() {
  m_color = Color::black;

  m_allValidMoves.clear();
  m_selectedMove = {};

  m_advantage = 0.0;
}

void AI::setAvailableMoves(const std::vector<FullMove> &validMoves) {
  // Should always reset before copying
  m_allValidMoves.clear();

  // For some reason, std::copy gives me weird template errors here
  for (const auto &move : validMoves) {
    m_allValidMoves.emplace_back(move);
  }

  auto checkForOpposingColor = [this](const FullMove &input) {
    if (m_color.has_value()) {
      return m_color != input.color;
    }

    return true;
  };

  // Don't need moves of opposing color here (for now?)
  m_allValidMoves.erase(std::remove_if(m_allValidMoves.begin(),
                                       m_allValidMoves.end(),
                                       checkForOpposingColor),
                        m_allValidMoves.end());
}

void AI::updateAdvantage() {
  const auto &currentState =
      m_boardAndGameStates[m_boardAndGameStates.size() - 1];

  for (size_t i = 0; i < currentState.pawns.size(); ++i) {
    m_advantage +=
        (currentState.pawns[i].first == Color::white ? k_pawnValue
                                                      : -k_pawnValue);
  }
  for (size_t i = 0; i < currentState.knights.size(); ++i) {
    m_advantage +=
        (currentState.knights[i].first == Color::white ? k_knightValue
                                                        : -k_knightValue);
  }
  for (size_t i = 0; i < currentState.bishops.size(); ++i) {
    m_advantage +=
        (currentState.bishops[i].first == Color::white ? k_bishopValue
                                                        : -k_bishopValue);
  }
  for (size_t i = 0; i < currentState.rooks.size(); ++i) {
    m_advantage +=
        (currentState.rooks[i].first == Color::white ? k_rookValue
                                                      : -k_rookValue);
  }
  for (size_t i = 0; i < currentState.queens.size(); ++i) {
    m_advantage +=
        (currentState.queens[i].first == Color::white ? k_queenValue
                                                       : -k_queenValue);
  }
  for (size_t i = 0; i < currentState.kings.size(); ++i) {
    m_advantage += (currentState.kings[i].first == Color::white ? k_kingValue
                                                                : -k_kingValue);
  }
}

double AI::calculateAdvantage(const LumpedBoardAndGameState &state) {
  double advantage = 0.0;

  for (size_t i = 0; i < state.pawns.size(); ++i) {
    advantage +=
        (state.pawns[i].first == Color::white) ? k_pawnValue : -k_pawnValue;
  }
  for (size_t i = 0; i < state.knights.size(); ++i) {
    advantage += (state.knights[i].first == Color::white) ? k_knightValue
                                                          : -k_knightValue;
  }
  for (size_t i = 0; i < state.bishops.size(); ++i) {
    advantage += (state.bishops[i].first == Color::white) ? k_bishopValue
                                                          : -k_bishopValue;
  }
  for (size_t i = 0; i < state.rooks.size(); ++i) {
    advantage +=
        (state.rooks[i].first == Color::white) ? k_rookValue : -k_rookValue;
  }
  for (size_t i = 0; i < state.queens.size(); ++i) {
    advantage +=
        (state.queens[i].first == Color::white) ? k_queenValue : -k_queenValue;
  }
  for (size_t i = 0; i < state.kings.size(); ++i) {
    advantage +=
        (state.kings[i].first == Color::white) ? k_kingValue : -k_kingValue;
  }

  return advantage;
}

std::pair<Position, Position> AI::getRandomMove() {
  auto result =
      selectRandomly(m_allValidMoves.cbegin(), m_allValidMoves.cend());
  m_selectedMove = {result->start, result->end};
  return m_selectedMove;
}

const LumpedBoardAndGameState AI::tryMove(size_t index) const {
  LumpedBoardAndGameState toReturn =
      m_boardAndGameStates[static_cast<int>(m_boardAndGameStates.size() - 1)];

  const auto &moveToTry = m_allValidMoves[index];

  for (size_t i = 0; i < toReturn.pawns.size(); ++i) {
    // Pawn match
    if (toReturn.pawns[i].second == moveToTry.start) {
      toReturn.pawns[i].second = moveToTry.end;
    }

    // Another pawn got taken
    if (toReturn.pawns[i].second == moveToTry.end) {
      toReturn.pawns.erase(toReturn.pawns.begin() + i);
    }
    // I don't think this is actually necessary
    // } else if (toReturn.enPassantTarget.has_value()) {
    //   if (toReturn.enPassantTarget.value() == moveToTry.end) {
    //     toReturn.pawns.erase(toReturn.pawns.begin() + i);
    //   }
    // }
  }
  
  for (size_t i = 0; i < toReturn.knights.size(); ++i) {
    // Knight match
    if (toReturn.knights[i].second == moveToTry.start) {
      toReturn.knights[i].second = moveToTry.end;
    }

    // Another knight got taken
    if (toReturn.knights[i].second == moveToTry.end) {
      toReturn.knights.erase(toReturn.knights.begin() + i);
    }
  }

  for (size_t i = 0; i < toReturn.bishops.size(); ++i) {
    // Bishop match
    if (toReturn.bishops[i].second == moveToTry.start) {
      toReturn.bishops[i].second = moveToTry.end;
    }

    // Another bishop got taken
    if (toReturn.bishops[i].second == moveToTry.end) {
      toReturn.bishops.erase(toReturn.bishops.begin() + i);
    }
  }

  for (size_t i = 0; i < toReturn.rooks.size(); ++i) {
    // Rook match
    if (toReturn.rooks[i].second == moveToTry.start) {
      toReturn.rooks[i].second = moveToTry.end;
    }

    // Another rook got taken
    if (toReturn.rooks[i].second == moveToTry.end) {
      toReturn.rooks.erase(toReturn.rooks.begin() + i);
    }
  }

  for (size_t i = 0; i < toReturn.queens.size(); ++i) {
    // Queen match
    if (toReturn.queens[i].second == moveToTry.start) {
      toReturn.queens[i].second = moveToTry.end;
    }

    // Another queen got taken
    if (toReturn.queens[i].second == moveToTry.end) {
      toReturn.queens.erase(toReturn.queens.begin() + i);
    }
  }

  for (size_t i = 0; i < toReturn.kings.size(); ++i) {
    // King match
    if (toReturn.kings[i].second == moveToTry.start) {
      toReturn.kings[i].second = moveToTry.end;
    }

    // ...should never happen?
    if (toReturn.kings[i].second == moveToTry.end) {
      toReturn.kings.erase(toReturn.kings.begin() + i);
    }
  }

  return toReturn;
}