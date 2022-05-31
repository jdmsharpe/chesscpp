#ifndef AI_H
#define AI_H

#include "Defs.h"

// Class that represents a computer player that a user can play against
class AI {
public:
  AI() {}
  ~AI() {}

  void reset();

  void setAvailableMoves(const std::vector<FullMove> &validMoves);
  inline void setBoardAndGameState(const LumpedBoardAndGameState &states) {
    m_boardAndGameStates.emplace_back(states);
  }

  void updateAdvantage();
  double calculateAdvantage(const LumpedBoardAndGameState &state);
  std::pair<Position, Position> getRandomMove();

  const LumpedBoardAndGameState tryMove(size_t index) const;

  inline std::optional<Color> getColor() { return m_color; }
  inline void setColor(Color color) { m_color = color; }

  inline std::vector<FullMove> getAllValidMoves() { return m_allValidMoves; }
  inline size_t getValidMoveSize() { return m_allValidMoves.size(); }
  inline const FullMove &getValidMoveAt(const int index) {
    return m_allValidMoves[index];
  }

private:
  // For now, hardcode
  // TODO: Let player choose their color and set computer to be opposite
  std::optional<Color> m_color = Color::black;

  // Stores all valid moves for the computer color
  std::vector<FullMove> m_allValidMoves = {};

  std::vector<LumpedBoardAndGameState> m_boardAndGameStates = {};

  std::pair<Position, Position> m_selectedMove = {};

  double m_advantage = 0.0;
};

#endif // AI_H