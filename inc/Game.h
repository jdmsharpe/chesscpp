#ifndef GAME_H
#define GAME_H

#include <cctype>
#include <regex>
#include <unordered_map>

#include "Defs.h"

class Game {
public:
  Game() {}
  ~Game() {}

  inline bool isInProgress() const { return m_inProgress; }

  inline void endWithDraw() {
    m_inProgress = false;
  }

  inline void endWithVictory() {
    m_inProgress = false;
    m_winner = whoseTurnIsIt();
  }

  void whoWon() const;

  inline Color whoseTurnIsIt() const {
    return m_whiteToMove ? Color::white : Color::black;
  }

  inline Color whoseTurnIsItNot() const {
    return m_whiteToMove ? Color::black : Color::white;
  }

  inline void switchPlayers() { m_whiteToMove = !m_whiteToMove; }

  void outputPlayerTurn() const;

  bool parseMove(const std::pair<std::string, std::string> &input,
                 std::pair<Position, Position> &output) const;

  bool parsePromotion(const std::string &input, PieceType &output) const;

  void outputMoveFormat() const;

  void outputPromotionRules() const;

  BoardLayout parseFen();

private:
  inline void setTurn(Color color) {
    if (color == Color::black) {
      m_whiteToMove = false;
    } else {
      m_whiteToMove = true;
    }
  }

  bool m_inProgress = true;
  bool m_whiteToMove = true;
  std::optional<Color> m_winner;
};

#endif // GAME_H