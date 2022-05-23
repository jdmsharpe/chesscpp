#include "Application.h"

#include <chrono>

namespace {

// Parse CLI arguments
bool argumentPassed(char **start, char **end, const std::string &toFind) {
  return std::find(start, end, toFind) != end;
}

} // namespace

Application::Application(int argc, char **argv)
    : m_window(Window()), m_appState(AppState::UNKNOWN) {
  // Passing "-l" as an additional argument loads the FEN stored in inc/load.fen
  if (argumentPassed(argv, argv + argc, "-l")) {
    m_window.loadFen();
  } else {
    m_window.loadGame();
  }

  // Passing "-v" as an additional argument enables debug logs
  if (argumentPassed(argv, argv + argc, "-v")) {
    k_verbose = true;
  }
}

int Application::run() {
  // To handle SDL events
  SDL_Event e;

  bool quit = false;

  while (!quit) {
    while (SDL_PollEvent(&e)) {
      // User closes window
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_MOUSEBUTTONDOWN:
        m_window.handleMouseInput(e.button);
        break;
      case SDL_KEYDOWN:
        m_window.handleKeyboardInput(e.key);
        break;
      }
    }

    switch (m_appState) {
    case AppState::GAME_IN_PROGRESS:
      m_window.stepGame();
      break;
    case AppState::GAME_COMPLETE:
      m_window.endGame();
      break;
    }

    m_window.render();
  }

  return 0;
}
