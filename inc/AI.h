#ifndef AI_H
#define AI_H

#include "Defs.h"

// Class that represents a computer player that a user can play against
class AI {
public:
  AI() {}
  ~AI() {}

  void setAvailableMoves(const std::vector<FullMove> &validMoves);
  inline void setBoardAndGameState(const LumpedBoardAndGameState& states) {
      m_boardAndGameState = states;
  }

  std::pair<Position, Position> calculateMove();

  inline std::optional<Color> getColor() { return m_color; }
  inline void setColor(Color color) { m_color = color; }

private:
  // For now, hardcode
  // TODO: Let player choose their color and set computer to be opposite
  std::optional<Color> m_color = Color::black;

  // Stores all valid moves for the computer color
  std::vector<FullMove> m_allValidMoves = {};

  LumpedBoardAndGameState m_boardAndGameState = {};
};

#endif // AI_H