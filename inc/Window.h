#ifndef WINDOW_H
#define WINDOW_H

#include "Board.h"
#include "Game.h"

class Window {
public:
  Window() : m_board(Board()), m_game(Game()) { open(); }
  ~Window() { close(); }

  void open();
  void close();

  void render() const;

  void handleMouseInput(const SDL_MouseButtonEvent& mbe);
  void handleKeyboardInput(const SDL_KeyboardEvent& kbe);

  inline void loadGame() { m_board.loadGame(); }
  inline void loadFen() { m_board.loadFromFen(m_game.parseFen()); }

  inline bool isGameInProgress() { return m_game.isInProgress(); }

  void stepGame();
  void endGame();

private:
  SDL_Renderer *m_sdlRenderer;
  SDL_Window *m_sdlWindow;
  SDL_Surface *m_sdlSurface;

  Board m_board;
  Game m_game;

  std::pair<std::string, std::string> m_moveInput = {};
  std::pair<Position, Position> m_moveOutput = {};
  std::string m_promotionInput = "";
  PieceType m_promotionOutput = PieceType::none;
};

#endif // WINDOW_H
