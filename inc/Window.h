#ifndef WINDOW_H
#define WINDOW_H

#include "Board.h"
#include "Game.h"

#include <queue>

class Window {
public:
  Window(const bool isLegacyMode);
  ~Window();

  void open();
  void close();

  void render();

  void handleMouseInput(const SDL_MouseButtonEvent &mbe);
  void handleKeyboardInput(const SDL_KeyboardEvent &kbe);

  inline void loadGame() { m_board.loadGame(); }
  inline void loadFen() { m_board.loadFromFen(m_game.parseFen()); }

  inline bool isGameInProgress() { return m_game.isInProgress(); }

  void stepGame();
  void endGame();

private:
  Board m_board;
  Game m_game;

  bool m_legacyMode = false;

  // Parameters exclusive to graphical mode
  SDL_Renderer *m_sdlRenderer;
  SDL_Window *m_sdlWindow;
  SDL_Surface *m_sdlSurface;
  std::queue<Position> m_clickedPositionQueue = {};

  // Parameters exclusive to legacy mode
  // TODO: Maybe refactor legacy mode into a separate class?
  std::pair<std::string, std::string> m_moveInput = {};
  std::pair<Position, Position> m_moveOutput = {};
  std::string m_promotionInput = "";
  PieceType m_promotionOutput = PieceType::none;
};

#endif // WINDOW_H
