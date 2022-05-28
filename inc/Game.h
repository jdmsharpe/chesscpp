#ifndef GAME_H
#define GAME_H

#include "Defs.h"

class Game {
public:
  Game() {}
  ~Game() {}

  inline bool isInProgress() const { return m_inProgress; }

  inline void endWithDraw() { m_inProgress = false; }

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

  inline size_t whatTurnIsIt() const {
    return m_turnNum;
  }

  inline void switchPlayers() {
    m_whiteToMove = !m_whiteToMove;
    ++m_halfMoveNum;
    // Modulo on size_t is badness, apparently
    if (static_cast<int>(m_halfMoveNum) % 2 == 0) {
      ++m_turnNum;
    }
  }

  void outputPlayerTurn() const;

  bool parseMove(const std::pair<std::string, std::string> &input,
                 std::pair<Position, Position> &output) const;

  bool parsePromotion(const std::string &input, PieceType &output) const;

  void outputMoveFormat() const;

  void outputPromotionRules() const;

  void outputKingInCheck() const;

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
  std::optional<Color> m_winner = std::nullopt;
  mutable bool m_displayWinnerOnceFlag  = true;

  size_t m_fiftyMoveRuleNum = 0;
  size_t m_halfMoveNum = 0;
  size_t m_turnNum = 1;
};

#endif // GAME_H
