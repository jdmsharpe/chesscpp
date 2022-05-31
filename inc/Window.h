#ifndef WINDOW_H
#define WINDOW_H

#include "AI.h"
#include "Board.h"
#include "Game.h"

// Class that represents the window in which the game is being played
// Handles user input and both owns and updates the board and game states
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
  inline void loadFen() { m_board.loadFromState(m_game.parseFen()); }

  inline bool isGameInProgress() { return m_game.isInProgress(); }

  void stepGame();
  void endGame();

  inline bool isComputerPlaying() const { return m_isComputerPlaying; }
  inline void setComputerPlaying(const bool isPlaying) {
    m_isComputerPlaying = isPlaying;
  }

private:
  void stepSdlGame();
  void stepLegacyGame();

  bool makePlayerMove();
  bool makeComputerMove();

  std::pair<FullMove, double> minimaxRoot(Color maximizer, int depth);
  double minimax(Color maximizer, int depth, double alpha, double beta);

  Board m_board;
  Game m_game;
  AI m_computer;

  // True if legacy mode is enabled
  bool m_legacyMode = false;

  // True if the computer is a player
  bool m_isComputerPlaying = false;

  // -------------- Parameters exclusive to graphical mode --------------
  SDL_Renderer *m_sdlRenderer;
  SDL_Window *m_sdlWindow;
  SDL_Surface *m_sdlSurface;

  //
  std::queue<Position> m_clickedPositionQueue = {};

  // -------------- Parameters exclusive to legacy mode --------------
  // TODO: Maybe refactor legacy mode into a separate class?

  // Stores user input move
  std::pair<std::string, std::string> m_moveInput = {};

  // Parsed move to be checked for validity
  std::pair<Position, Position> m_moveOutput = {};

  // Stores user input promotion choice
  std::string m_promotionInput = "";

  // Parsed promotion choice to be applied
  PieceType m_promotionOutput = PieceType::none;
};

#endif // WINDOW_H
