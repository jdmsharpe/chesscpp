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

  inline Color whoseTurnIsIt() const {
    return m_whiteToMove ? Color::white : Color::black;
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
};

#endif // GAME_H