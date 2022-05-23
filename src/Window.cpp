#include "Window.h"

#include "Defs.h"

namespace {

// Define how long one frame should be
// 16 ms is equivalent to ~60 FPS (really 62.5 FPS)
constexpr int k_dt = 16; // ms

// SDL window default dimensions
constexpr int k_windowWidth = 1068;
constexpr int k_windowHeight = 762;

constexpr int k_boxWidth = k_windowWidth / 16;
constexpr int k_boxHeight = k_windowHeight / 16;

} // namespace

// Boilerplate SDL code, for the most part

void Window::open() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "SDL could not initialize! Error code: %s.", SDL_GetError());
  } else {
    // Create window
    m_sdlWindow = SDL_CreateWindow("Chess in C++", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, k_windowWidth,
                                   k_windowHeight, 0);
    if (m_sdlWindow == NULL) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Window could not be created. Error code: %s.",
                   SDL_GetError());
    } else {
      m_sdlRenderer =
          SDL_CreateRenderer(m_sdlWindow, -1, SDL_RENDERER_SOFTWARE);
      m_sdlSurface = SDL_GetWindowSurface(m_sdlWindow);
    }
  }

  // Something really went wrong
  if (m_sdlRenderer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "MainMenu pointer to renderer was NULL!");
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Image could not initialize!");
  }

  m_board.setRenderer(m_sdlRenderer);
}

void Window::close() {
  // Clean up all entities and SDL objects
  SDL_FreeSurface(m_sdlSurface);
  m_sdlSurface = NULL;

  SDL_DestroyWindow(m_sdlWindow);
  m_sdlWindow = NULL;

  SDL_DestroyRenderer(m_sdlRenderer);
  m_sdlRenderer = NULL;

  SDL_Quit();
}

void Window::render() const {
  Uint32 frameStart = SDL_GetTicks();

  // Black background
  SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(m_sdlRenderer);

  SDL_RenderPresent(m_sdlRenderer);

  Uint32 frameTime = SDL_GetTicks() - frameStart;

  // If actual time taken is less than defined frame time,
  // wait a bit until they're equal (fixed framerate)
  if (k_dt > frameTime) {
    // Debug print to check actual elapsed time
    // SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Elapsed frame time was %d
    // ms.",
    //             frameTime);
    SDL_Delay(k_dt - frameTime);
  }
}

void Window::handleMouseInput(const SDL_MouseButtonEvent &mbe) {
  int x;
  int y;
  int boardX;
  int boardY;

  if (mbe.button == SDL_BUTTON_LEFT) {
    SDL_GetMouseState(&x, &y);

    if ((x >= 0 && x < k_windowWidth) && (y >= 0 && y < k_windowHeight)) {
    }
  }
}

void Window::handleKeyboardInput(const SDL_KeyboardEvent &kbe) {}

void Window::stepGame() {
  // TODO: Deprecate original display function or put into verbose
  m_board.cliDisplay(m_game.whoseTurnIsIt());

  if (m_board.isKingInCheck(m_game.whoseTurnIsIt())) {
    // Alert player if their king is in check
    m_game.outputKingInCheck();
  }

  m_game.outputPlayerTurn();

  std::cin >> m_moveInput.first >> m_moveInput.second;

  // For timing and later optimization
  auto startTurn = std::chrono::system_clock::now();

  m_game.parseMove(m_moveInput, m_moveOutput);

  if (m_board.isValidMove(m_game.whoseTurnIsIt(), m_moveOutput.first,
                          m_moveOutput.second, false)) {
    m_board.movePiece(m_moveOutput.first, m_moveOutput.second);
    m_board.updateBoardState(m_moveOutput.first, m_moveOutput.second);

    while (m_board.pawnToPromote()) {
      m_game.outputPromotionRules();
      std::cin >> m_promotionInput;
      if (m_game.parsePromotion(m_promotionInput, m_promotionOutput)) {
        m_board.promotePawn(m_promotionOutput);
      }
    }

    if (m_board.isKingCheckmated(m_game.whoseTurnIsItNot())) {
      m_game.endWithVictory();
    } else if (m_board.hasStalemateOccurred(m_game.whoseTurnIsItNot())) {
      m_game.endWithDraw();
    }

    // When move is complete, turn is over
    m_game.switchPlayers();
  }

  auto endTurn = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = endTurn - startTurn;
  if (k_verbose) {
    std::cout << "DEBUG: Time to complete turn was " << diff.count()
              << " seconds." << std::endl;
  }
}

void Window::endGame() {
  m_board.cliDisplay(m_game.whoseTurnIsIt());
  m_game.whoWon();
}
