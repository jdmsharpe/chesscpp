#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"

// Defines all possible application states
enum class AppState {
  UNKNOWN = -1,
  MENU = 0,
  GAME_IN_PROGRESS = 1,
  GAME_COMPLETE = 2
};

// Class for the game application
// Responsible for opening and managing the SDL window,
// starting and stepping the game, and parsing in CLI arguments
class Application {
public:
  Application(int argc, char **argv);
  virtual ~Application() = default;

  // Disallow copy and assign
  Application(const Application &) = delete;
  void operator=(const Application &) = delete;

  int run();

private:
  // Pointer to Window class
  std::unique_ptr<Window> m_window = nullptr;

  // Current application state
  AppState m_appState = AppState::UNKNOWN;

  // True if legacy mode is enabled
  bool m_legacyMode = false;

  bool m_saveGames = false;
};

#endif // APPLICATION_H
