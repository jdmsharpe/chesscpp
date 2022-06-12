#include "Window.h"

#include "Defs.h"

namespace {

// Define how long one frame should be
// 16 ms is equivalent to ~60 FPS (really 62.5 FPS)
constexpr int k_dt = 16; // ms

constexpr int k_checkmateValue = 2000;

Position convertMouseInputToPosition(const int x, const int y) {
  return {x / k_squareWidth, std::abs((y / k_squareWidth))};
}

} // namespace

Window::Window(const bool isLegacyMode)
    : m_board(Board()), m_game(Game()), m_computer(AI(m_board)),
      m_legacyMode(isLegacyMode) {
  if (!m_legacyMode) {
    open();
  }
}

Window::~Window() {
  if (!m_legacyMode) {
    close();
  }
}

// Boilerplate SDL code
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
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Pointer to renderer was NULL!");
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Image could not initialize!");
  }

  SDL_SetRenderDrawBlendMode(m_sdlRenderer, SDL_BLENDMODE_BLEND);

  m_board.setRenderer(m_sdlRenderer);
  m_board.loadTextures();
}

void Window::close() {
  // Clean up all entities and SDL objects
  SDL_FreeSurface(m_sdlSurface);
  m_sdlSurface = NULL;

  SDL_DestroyWindow(m_sdlWindow);
  m_sdlWindow = NULL;

  SDL_DestroyRenderer(m_sdlRenderer);
  m_sdlRenderer = NULL;

  IMG_Quit();
  SDL_Quit();
}

void Window::render() {
  Uint32 frameStart = SDL_GetTicks();

  // Grey background
  SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(m_sdlRenderer);

  m_board.sdlDisplay();

  SDL_RenderPresent(m_sdlRenderer);

  Uint32 frameTime = SDL_GetTicks() - frameStart;

  // If actual time taken is less than defined frame time,
  // wait a bit until they're equal (fixed framerate)
  if (k_dt > frameTime) {
    // Debug print to check actual elapsed time
    // SDL_Log("Elapsed frame time was %d ms.", frameTime);
    SDL_Delay(k_dt - frameTime);
  }
}

void Window::handleMouseInput(const SDL_MouseButtonEvent &mbe) {
  int x;
  int y;
  int boardX;
  int boardY;

  Uint32 buttons;

  if (mbe.button == SDL_BUTTON_LEFT) {
    SDL_GetMouseState(&x, &y);

    // No inputs out of bounds allowed
    RETURN_IF_VALID((x < 0 || x >= k_windowWidth) ||
                    (y < 0 || y >= k_windowHeight));

    // Push back to storage queue
    m_clickedPositionQueue.push(convertMouseInputToPosition(x, y));
  }
}

void Window::handleKeyboardInput(const SDL_KeyboardEvent &kbe) {
  const Uint8 *kb = SDL_GetKeyboardState(NULL);

  if (kbe.keysym.sym == SDLK_r) {
    m_board.loadGame();
    m_board.refreshValidMoves();
    m_board.clearOldKingHighlight();
    m_game.reset();

    if (m_computer.getColor().has_value()) {
      m_computer.reset();
    }
  }
}

void Window::stepGame() {
  if (m_legacyMode) {
    stepLegacyGame();
  } else {
    stepSdlGame();
  }
}

void Window::stepLegacyGame() {
  // Legacy mode uses CLI inputs
  m_board.cliDisplay(m_game.whoseTurnIsIt());

  if (m_board.isKingInCheck(m_game.whoseTurnIsIt())) {
    // Alert player if their king is in check
    m_game.outputKingInCheck();
  }

  m_game.outputPlayerTurn();

  std::cin >> m_moveInput.first >> m_moveInput.second;

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
}

void Window::stepSdlGame() {
  if (m_board.isKingInCheck(m_game.whoseTurnIsIt())) {
    // Alert player if their king is in check
    m_board.highlightKingInCheck(m_game.whoseTurnIsIt());
  }

  if (m_isComputerPlaying) {
    if (m_game.whoseTurnIsIt() != m_computer.getColor()) {
      if (!makePlayerMove()) {
        return;
      }
    } else {
      if (!makeComputerMove()) {
        return;
      }
    }
  } else {
    if (!makePlayerMove()) {
      return;
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

void Window::endGame() {
  // m_board.cliDisplay(m_game.whoseTurnIsIt());
  m_game.whoWon();
}

bool Window::makePlayerMove() {
  // Ensures valid moves are populated on first turn
  if (m_game.whatTurnIsIt() <= 1) {
    m_board.refreshValidMoves();
  }

  // Standard mode uses SDL for graphics
  if (m_clickedPositionQueue.size() < 1) {
    // No input to process
    m_board.clearOldPieceHighlight();
    return false;
  }

  if (!m_board.isInputValid(m_game.whoseTurnIsIt(), m_clickedPositionQueue)) {
    m_clickedPositionQueue.pop();
    return false;
  }

  if (m_clickedPositionQueue.size() < 2) {
    m_board.highlightPotentialMoves(m_clickedPositionQueue.front());
    return false;
  }

  // LMB was clicked twice and two valid positions were stored
  Position firstPosition = m_clickedPositionQueue.front();
  m_clickedPositionQueue.pop();

  Position secondPosition = m_clickedPositionQueue.front();
  m_clickedPositionQueue.pop();

  if (m_board.isValidMove(m_game.whoseTurnIsIt(), firstPosition, secondPosition,
                          false)) {
    m_board.movePiece(firstPosition, secondPosition);
    m_board.updateBoardState(firstPosition, secondPosition);

    while (m_board.pawnToPromote()) {
      m_game.outputPromotionRules();
      std::cin >> m_promotionInput;
      if (m_game.parsePromotion(m_promotionInput, m_promotionOutput)) {
        m_board.promotePawn(m_promotionOutput);
        break;
      }
    }

    m_board.refreshValidMoves();

    return true;

  } else {
    // Not a valid move
    return false;
  }
}

bool Window::makeComputerMove() {
  // Simple sleep to not make moves almost instantaneous
  // Can probably perform this some other way but this works for now
  // TODO: Has a bug where computer's king doesn't highlight when in check
  // std::this_thread::sleep_for(std::chrono::milliseconds(300));

  if (!m_computer.getColor().has_value()) {
    return false;
  }

  Color computerColor = m_computer.getColor().value();

  const auto &bestMove = m_computer.minimaxRoot(computerColor);

  std::cout << "The move was from: " << bestMove.first.first << " "
            << bestMove.first.second << " to: " << bestMove.second.first << " "
            << bestMove.second.second << std::endl;

  // This call is enforced because the function is responsible for the move in
  // some cases
  if (m_board.isValidMove(computerColor, bestMove.first, bestMove.second,
                          false)) {
    m_board.movePiece(bestMove.first, bestMove.second);
    m_board.updateBoardState(bestMove.first, bestMove.second);

    if (m_board.pawnToPromote()) {
      // I'm not smart enough to code for situations where a queen isn't the
      // best piece for promotion
      m_board.promotePawn(PieceType::queen);
    }

    m_board.refreshValidMoves();

    return true;
  }

  return false;
}
