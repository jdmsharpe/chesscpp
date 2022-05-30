#ifndef GAME_H
#define GAME_H

#include "Defs.h"

// Class to define the game state
// Handles turns and game completion, and is 
// responsible for parsing moves and FEN
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

  inline size_t whatTurnIsIt() const { return m_turnNum; }

  inline size_t getHalfMoveCount() const { return m_halfMoveNum; }

  inline size_t getMoveCount() const { return m_turnNum; }

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

  LumpedBoardAndGameState parseFen();

private:
  inline void setTurn(Color color) {
    if (color == Color::black) {
      m_whiteToMove = false;
    } else {
      m_whiteToMove = true;
    }
  }

  // True if there is a game in progress
  bool m_inProgress = true;

  // True if it's white's turn
  bool m_whiteToMove = true;

  // Winner of the game, if any
  std::optional<Color> m_winner = std::nullopt;

  // Flag so the result doesn't spam
  mutable bool m_displayWinnerOnceFlag = true;

  // How many turns have passed since pawn move or capture
  size_t m_fiftyMoveRuleNum = 0;

  // How many turns each player has taken in total, i.e. combined black and
  // white
  size_t m_halfMoveNum = 0;

  // Turn number
  size_t m_turnNum = 1;
};

#endif // GAME_H
