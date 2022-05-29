#include "AI.h"

#include  <random>
#include  <iterator>

namespace {

// Source: https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
template<typename Iter, typename RandomGenerator>
Iter selectRandomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter selectRandomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return selectRandomly(start, end, gen);
}


constexpr int k_pawnValue = 1;
constexpr int k_knightValue = 3;
constexpr int k_bishopValue = 3;
constexpr int k_rookValue = 5;
constexpr int k_queenValue = 9;
constexpr int k_kingValue = 100;

} // namespace

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



std::pair<Position, Position> AI::calculateMove() {
  auto result =
      selectRandomly(m_allValidMoves.cbegin(), m_allValidMoves.cend());
  return {result->start, result->end};
}