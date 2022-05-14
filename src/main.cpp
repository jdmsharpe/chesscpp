#include "Board.h"
#include "Game.h"

// Parse CLI arguments
bool argumentPassed(char **start, char **end, const std::string &toFind) {
  return std::find(start, end, toFind) != end;
}

int main(int argc, char **argv) {
  // Stack is potentially faster? But maybe these should be on the heap
  Board board;
  Game game;

  // Passing "-l" as an additional argument loads the FEN stored in inc/load.fen
  if (argumentPassed(argv, argv + argc, "-l")) {
    board.loadFromFen(game.parseFen());
  } else {
    board.loadGame();
  }

  // Initialize containers for player inputs/parsed outputs
  std::pair<std::string, std::string> moveInput;
  std::pair<Position, Position> moveOutput;
  std::string promotionInput;
  PieceType promotionOutput;

  while (game.isInProgress()) {
    // // Clear terminal (should investigate a better way to do this)
    // std::cout << "\033[H\033[2J\033[3J";

    board.display();
    game.outputPlayerTurn();
    std::cin >> moveInput.first >> moveInput.second;

    game.parseMove(moveInput, moveOutput);

    if (board.isValidMove(game.whoseTurnIsIt(), moveOutput.first, moveOutput.second)) {
      board.movePiece(moveOutput.first, moveOutput.second);
      board.updateAfterMove(moveOutput.first, moveOutput.second);

      while (board.pawnToPromote()) {
        game.outputPromotionRules();
        std::cin >> promotionInput;
        if (game.parsePromotion(promotionInput, promotionOutput)) {
          board.promotePawn(promotionOutput);
        }
      }

      // board.isKingCheckmated(game.whoseTurnIsIt());

      // When move is complete, turn is over
      game.switchPlayers();
    }
  }

  return 0;
}
