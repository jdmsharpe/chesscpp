#include "Application.h"

#include <chrono>

namespace {

// Parse CLI arguments
bool argumentPassed(char **start, char **end, const std::string &toFind) {
  return std::find(start, end, toFind) != end;
}

} // namespace

Application::Application(int argc, char **argv)
    : m_appState(AppState::UNKNOWN) {
  // Passing "--legacy" as an additional argument enables CLI legacy mode
  if (argumentPassed(argv, argv + argc, "--legacy")) {
    m_legacyMode = true;
  }

  m_window = std::make_unique<Window>(m_legacyMode);

  // Passing "-l" as an additional argument loads the FEN stored in inc/load.fen
  if (argumentPassed(argv, argv + argc, "-l")) {
    m_window->loadFen();
  } else {
    m_window->loadGame();
  }

  // Passing "-v" as an additional argument enables debug logs
  if (argumentPassed(argv, argv + argc, "-v")) {
    k_verbose = true;
  }

  m_appState = AppState::GAME_IN_PROGRESS;
}

int Application::run() {
  // To handle SDL events
  SDL_Event e;

  // Initialize tracking variables
  bool quit = false;
  bool gameComplete = false;
  std::chrono::duration<double> diff;
  std::chrono::_V2::system_clock::time_point runStart;
  std::chrono::_V2::system_clock::time_point runEnd;

  while (!quit) {
    runStart = std::chrono::system_clock::now();
    // All SDL tasks are exclusive to new mode
    if (!m_legacyMode) {
      while (SDL_PollEvent(&e)) {
        // User closes window
        switch (e.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_MOUSEBUTTONDOWN:
          m_window->handleMouseInput(e.button);
          break;
        case SDL_KEYDOWN:
          m_window->handleKeyboardInput(e.key);
          break;
        }
      }
    }

    switch (m_appState) {
    case AppState::GAME_IN_PROGRESS:
      // For timing and later optimization

      m_window->stepGame();

      if (!m_window->isGameInProgress()) {
        m_appState = AppState::GAME_COMPLETE;
      }

      break;

    case AppState::GAME_COMPLETE:
      m_window->endGame();
      break;
    }

    m_window->render();

    runEnd = std::chrono::system_clock::now();
    diff = runEnd - runStart;
    if (k_verbose) {
      SDL_Log("Time to complete run() was %.2f ms.", diff.count() * 1000);
    }
  }

  return 0;
}
