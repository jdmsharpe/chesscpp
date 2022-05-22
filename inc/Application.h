#ifndef APPLICATION_H
#define APPLICATION_H

#include "Board.h"
#include "Game.h"

class Application {
public:
  Application(int argc, char **argv);
  virtual ~Application() = default;

  Application(const Application &) = delete;
  Application(Application &) = delete;

  int run();

private:
  Board m_board;
  Game m_game;

  std::pair<std::string, std::string> m_moveInput = {};
  std::pair<Position, Position> m_moveOutput = {};
  std::string m_promotionInput = "";
  PieceType m_promotionOutput = PieceType::none;
};

#endif // APPLICATION_H