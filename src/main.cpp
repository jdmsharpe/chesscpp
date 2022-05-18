#include "Board.h"
#include "Game.h"

#include <chrono>

// Parse CLI arguments
bool argumentPassed(char **start, char **end, const std::string &toFind) {
  return std::find(start, end, toFind) != end;
}

int main(int argc, char **argv) {
  Board board;
  Game game;

  // Passing "-l" as an additional argument loads the FEN stored in inc/load.fen
  if (argumentPassed(argv, argv + argc, "-l")) {
    board.loadFromFen(game.parseFen());
  } else {
    board.loadGame();
  }

  // Passing "-v" as an additional argument enables debug logs
  if (argumentPassed(argv, argv + argc, "-v")) {
    k_verbose = true;
  }

  // Initialize containers for player inputs/parsed outputs
  std::pair<std::string, std::string> moveInput;
  std::pair<Position, Position> moveOutput;
  std::string promotionInput;
  PieceType promotionOutput;

  while (game.isInProgress()) {
    // Clear terminal (should investigate a better way to do this)
    std::cout << "\033[H\033[2J\033[3J";

    board.display(game.whoseTurnIsIt());
    if (board.isKingInCheck(game.whoseTurnIsIt())) {
      // Alert player if their king is in check
      game.outputKingInCheck();
    }
    game.outputPlayerTurn();

    std::cin >> moveInput.first >> moveInput.second;

    // For timing and later optimization
    auto startTurn = std::chrono::system_clock::now();

    game.parseMove(moveInput, moveOutput);

    if (board.isValidMove(game.whoseTurnIsIt(), moveOutput.first,
                          moveOutput.second, false)) {
      board.movePiece(moveOutput.first, moveOutput.second);
      board.updateBoardState(moveOutput.first, moveOutput.second);

      while (board.pawnToPromote()) {
        game.outputPromotionRules();
        std::cin >> promotionInput;
        if (game.parsePromotion(promotionInput, promotionOutput)) {
          board.promotePawn(promotionOutput);
        }
      }

      if (board.isKingCheckmated(game.whoseTurnIsItNot())) {
        game.endWithVictory();
      } else if (board.hasStalemateOccurred(game.whoseTurnIsItNot())) {
        game.endWithDraw();
      }

      // When move is complete, turn is over
      game.switchPlayers();
    }

    auto endTurn = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = endTurn - startTurn;
    if (k_verbose) {
      std::cout << "DEBUG: Time to complete turn was " << diff.count()
                << " seconds." << std::endl;
    }
  }

  board.display(game.whoseTurnIsIt());
  game.whoWon();

  return 0;
}
