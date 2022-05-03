#ifndef GAME_H
#define GAME_H

#include <cctype>
#include <regex>
#include <string>
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

  bool parseMove(const std::pair<std::string, std::string> input,
                 std::pair<Position, Position> &output) const;

  void explainMoveFormat() const;

  std::string parseFen() const;

private:
  bool m_inProgress = true;
  bool m_whiteToMove = true;
};

#endif // GAME_H