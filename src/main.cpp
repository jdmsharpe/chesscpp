#include "Board.h"
#include "Game.h"

bool argumentPassed(char **start, char **end, const std::string &toFind) {
  return std::find(start, end, toFind) != end;
}

int main(int argc, char **argv) {
  Board board;
  Game game;

  if (argumentPassed(argv, argv + argc, "-l")) {
    board.loadFromFen(game.parseFen());
  } else {
    board.loadGame();
  }

  // Initialize containers for player inputs
  std::pair<std::string, std::string> input;
  std::pair<Position, Position> output;

  while (game.isInProgress()) {
    // // Clear terminal (should investigate a better way to do this)
    // std::cout << "\033[H\033[2J\033[3J";

    std::cout << board.isKingInCheck(game.whoseTurnIsIt()) << std::endl;
    board.display();
    game.outputPlayerTurn();
    std::cin >> input.first >> input.second;

    game.parseMove(input, output);

    if (board.isValidMove(game.whoseTurnIsIt(), output.first, output.second)) {
      board.movePiece(output.first, output.second);

      // When move is complete, turn is over
      game.switchPlayers();
    }
  }

  return 0;
}
