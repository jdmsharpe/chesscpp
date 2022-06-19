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

  inline void setDifficulty(int increment) {
    // Min is indisputably 1, 0 causes a softlock
    // Max could be higher, but a depth of 5 is very slow (1 min or more)
    // so no real reason to go beyond it
    if (m_difficulty + increment < 1) {
      return;
    } else if (m_difficulty + increment > 5) {
      return;
    }

    m_difficulty += increment;
  }

  std::pair<Position, Position> minimaxRoot(Color max);
  int minimax(Color max, int depth, int alpha, int beta);

private:
  Board &m_board;

  // Default color is black if no flag is passed
  std::optional<Color> m_color = Color::black;

  int m_difficulty = 3;
};

#endif // AI_H