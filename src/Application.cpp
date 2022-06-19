#include "Application.h"

#include <chrono>
#include <iomanip>

namespace {

constexpr long k_counterMod = 10;

const std::string k_fenFilename = "../chess/inc/load.fen";
constexpr int k_firstFenIndex = 0;

const std::string k_writeFenPrefix = "game";

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
    m_window->loadFen(k_fenFilename, k_firstFenIndex);
  } else {
    m_window->loadGame();
  }

  // Passing "-v" as an additional argument enables debug logs
  if (argumentPassed(argv, argv + argc, "-v")) {
    k_verbose = true;
  }

  // Passing "-c" as an additional argument adds a computer player
  if (argumentPassed(argv, argv + argc, "-c")) {
    m_window->setComputerPlaying(true);
  }

  // Passing "-w" as an additional argument sets the active player to white
  if (argumentPassed(argv, argv + argc, "-w")) {
    m_window->setTurn(Color::white);
    if (m_window->isComputerPlaying()) {
      m_window->setComputerColor(Color::black);
    }
  }

  // Passing "-b" as an additional argument sets the active player to black
  if (argumentPassed(argv, argv + argc, "-b")) {
    m_window->setTurn(Color::white);
    if (m_window->isComputerPlaying()) {
      m_window->setComputerColor(Color::white);
    }
  }

  // Passing "-r" as an additional argument randomizes player and computer
  // player's colors
  if (argumentPassed(argv, argv + argc, "-r")) {
    m_window->setTurn(Color::white);
    if (m_window->isComputerPlaying()) {
      // Make rand() call pseudo-nondeterministic
      srand(time(NULL));
      (rand() % 2) ? m_window->setComputerColor(Color::white)
                   : m_window->setComputerColor(Color::black);
    }
  }

  // Passing "-b" as an additional argument sets the active player to black
  if (argumentPassed(argv, argv + argc, "-s")) {
    m_saveGames = true;
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

  int resetCount = 0;

  if (m_saveGames) {
    auto in_time_t =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream datetime;
    datetime << "_" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%X")
             << resetCount << ".fen";

    m_window->setActiveFilename(k_writeFenPrefix + datetime.str());
    datetime.str(std::string());
  }

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
          // TODO: Come up with a better way to do this
          if (e.key.keysym.sym == SDLK_r) {
            ++resetCount;
            m_appState = AppState::GAME_IN_PROGRESS;

            if (m_saveGames) {
              auto in_time_t = std::chrono::system_clock::to_time_t(
                  std::chrono::system_clock::now());
              std::stringstream datetime;
              datetime << "_"
                       << std::put_time(std::localtime(&in_time_t),
                                        "%Y-%m-%d-%X")
                       << resetCount << ".fen";

              m_window->setActiveFilename(k_writeFenPrefix + datetime.str());
              datetime.str(std::string());
            }
          }
          break;
        }
      }
    }

    switch (m_appState) {
    case AppState::GAME_IN_PROGRESS:
      m_window->stepGame();

      if (!m_window->isGameInProgress()) {
        m_appState = AppState::GAME_COMPLETE;
      }

      break;

    case AppState::GAME_COMPLETE:
      m_window->endGame();
      break;

    case AppState::MENU:
    case AppState::UNKNOWN:
      // Unimplemented for now
      break;
    }

    if (!m_legacyMode) {
      m_window->render();
    }

    runEnd = std::chrono::system_clock::now();
    diff = runEnd - runStart;
    if (k_verbose && (k_counter % k_counterMod == 0)) {
      printf("Time to complete run() was %.2f ms.\n", diff.count() * 1000);
    }
    ++k_counter;
  }

  return 0;
}
