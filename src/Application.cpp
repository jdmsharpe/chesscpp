#include "Application.h"

#include <chrono>

namespace {

// Parse CLI arguments
bool argumentPassed(char **start, char **end, const std::string &toFind) {
  return std::find(start, end, toFind) != end;
}

} // namespace

Application::Application(int argc, char **argv) : m_board(Board()), m_game(Game()) {
  // Passing "-l" as an additional argument loads the FEN stored in inc/load.fen
  if (argumentPassed(argv, argv + argc, "-l")) {
    m_board.loadFromFen(m_game.parseFen());
  } else {
    m_board.loadGame();
  }

  // Passing "-v" as an additional argument enables debug logs
  if (argumentPassed(argv, argv + argc, "-v")) {
    k_verbose = true;
  }
}

int Application::run() {
  while (m_game.isInProgress()) {
    // Clear terminal (should investigate a better way to do this)
    std::cout << "\033[H\033[2J\033[3J";

    m_board.display(m_game.whoseTurnIsIt());
    if (m_board.isKingInCheck(m_game.whoseTurnIsIt())) {
      // Alert player if their king is in check
      m_game.outputKingInCheck();
    }
    m_game.outputPlayerTurn();

    std::cin >> m_moveInput.first >> m_moveInput.second;

    // For timing and later optimization
    auto startTurn = std::chrono::system_clock::now();

    m_game.parseMove(m_moveInput, m_moveOutput);

    if (m_board.isValidMove(m_game.whoseTurnIsIt(), m_moveOutput.first,
                            m_moveOutput.second, false)) {
      m_board.movePiece(m_moveOutput.first, m_moveOutput.second);
      m_board.updateBoardState(m_moveOutput.first, m_moveOutput.second);

      while (m_board.pawnToPromote()) {
        m_game.outputPromotionRules();
        std::cin >> m_promotionInput;
        if (m_game.parsePromotion(m_promotionInput, m_promotionOutput)) {
          m_board.promotePawn(m_promotionOutput);
        }
      }

      if (m_board.isKingCheckmated(m_game.whoseTurnIsItNot())) {
        m_game.endWithVictory();
      } else if (m_board.hasStalemateOccurred(m_game.whoseTurnIsItNot())) {
        m_game.endWithDraw();
      }

      // When move is complete, turn is over
      m_game.switchPlayers();
    }

    auto endTurn = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = endTurn - startTurn;
    if (k_verbose) {
      std::cout << "DEBUG: Time to complete turn was " << diff.count()
                << " seconds." << std::endl;
    }
  }

  m_board.display(m_game.whoseTurnIsIt());
  m_game.whoWon();

  return 0;
}
