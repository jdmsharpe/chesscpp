#ifndef AI_H
#define AI_H

#include "Board.h"
#include "Defs.h"

// Class that represents a computer player that a user can play against
class AI {
public:
  AI(Board& board) : m_board(board) {}
  ~AI() {}

  void reset();

  int getAdvantage();
  std::pair<Position, Position> getRandomMove();

  inline std::optional<Color> getColor() { return m_color; }
  inline void setColor(Color color) { m_color = color; }

  std::pair<Position, Position> minimaxRoot(Color max);
  int minimax(Color max, int depth, int alpha, int beta);

private:
  Board &m_board;

  // Default color is black if no flag is passed
  std::optional<Color> m_color = Color::black;
};

#endif // AI_H