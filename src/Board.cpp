#include "Board.h"

namespace {

const std::string k_pieceImageFilepath = "../chesscpp/inc/pieces.png";

constexpr int k_pawnsPerSide = k_totalPieces / 4;

// Dark green
constexpr SDL_Color k_evenColor = SDL_Color({118, 150, 86, SDL_ALPHA_OPAQUE});
// Beige-ish
constexpr SDL_Color k_oddColor = SDL_Color({238, 238, 210, SDL_ALPHA_OPAQUE});
// Green
constexpr SDL_Color k_movementOptionColor = SDL_Color({0, 255, 0, 128});
// Red
constexpr SDL_Color k_checkColor = SDL_Color({255, 0, 0, 128});
// Yellow
constexpr SDL_Color k_selectedPieceColor = SDL_Color({255, 255, 0, 128});
// Purple
constexpr SDL_Color k_blackLastMoveColor = SDL_Color({86, 29, 94, 100});
// Orange
constexpr SDL_Color k_whiteLastMoveColor = SDL_Color({244, 128, 55, 100});

constexpr int k_pieceWidth = 105;
constexpr int k_pieceHeight = 105;

constexpr int k_screenBoxSize = 100;
constexpr int k_screenBoxOffset = 5;

constexpr int k_xPawnOffset = 529;
constexpr int k_xRookOffset = 425;
constexpr int k_xKnightOffset = 317;
constexpr int k_xBishopOffset = 211;
constexpr int k_xQueenOffset = 105;
constexpr int k_xKingOffset = 0;
constexpr int k_xKingAdditionalOffset = 2;

constexpr int k_xPromotionOffset = 2;

constexpr std::array<Position, 8> k_potentialKnightPositions = {
    {{2, 1}, {2, -1}, {-2, -1}, {-2, 1}, {1, 2}, {1, -2}, {-1, -2}, {-1, 2}}};

template <class T>
std::unique_ptr<T> makePiece(Color color, const Position &position) {
  return std::move(std::make_unique<T>(position, color));
}

Position getDirectionVector(const Position &start, const Position &end) {
  return {end.first - start.first, end.second - start.second};
}

// Credit where credit is due:
// https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sign(T val) { return (T(0) < val) - (val < T(0)); }

} // namespace

Board::~Board() {
  SDL_DestroyTexture(m_pieceImageTexture);
  m_pieceImageTexture = NULL;
}

void Board::loadTextures() {
  // Load piece image as surface and convert to texture
  SDL_Surface *pieceImageSurface = IMG_Load(k_pieceImageFilepath.c_str());
  m_pieceImageTexture =
      SDL_CreateTextureFromSurface(m_renderer, pieceImageSurface);
  SDL_FreeSurface(pieceImageSurface);
}

void Board::loadGame() {
  m_pieces.clear();
  m_pieces.resize(2);
  for (size_t i = 0; i < 2; ++i) {
    m_pieces[i].resize(k_totalPieces / 2);
  }

  for (int i = 0; i < k_pawnsPerSide; ++i) {
    m_pieces[1][i] = makePiece<Pawn>(Color::white, {i, 1});
  }

  m_pieces[1][k_pawnsPerSide] = makePiece<Rook>(Color::white, {0, 0});
  m_pieces[1][k_pawnsPerSide + 1] = makePiece<Rook>(Color::white, {7, 0});
  m_pieces[1][k_pawnsPerSide + 2] = makePiece<Knight>(Color::white, {1, 0});
  m_pieces[1][k_pawnsPerSide + 3] = makePiece<Knight>(Color::white, {6, 0});
  m_pieces[1][k_pawnsPerSide + 4] = makePiece<Bishop>(Color::white, {2, 0});
  m_pieces[1][k_pawnsPerSide + 5] = makePiece<Bishop>(Color::white, {5, 0});
  m_pieces[1][k_pawnsPerSide + 6] = makePiece<Queen>(Color::white, {3, 0});
  m_pieces[1][k_pawnsPerSide + 7] = makePiece<King>(Color::white, {4, 0});

  for (int i = 0; i < k_pawnsPerSide; ++i) {
    m_pieces[0][i] = makePiece<Pawn>(Color::black, {i, 6});
  }

  m_pieces[0][k_pawnsPerSide] = makePiece<Rook>(Color::black, {0, 7});
  m_pieces[0][k_pawnsPerSide + 1] = makePiece<Rook>(Color::black, {7, 7});
  m_pieces[0][k_pawnsPerSide + 2] = makePiece<Knight>(Color::black, {1, 7});
  m_pieces[0][k_pawnsPerSide + 3] = makePiece<Knight>(Color::black, {6, 7});
  m_pieces[0][k_pawnsPerSide + 4] = makePiece<Bishop>(Color::black, {2, 7});
  m_pieces[0][k_pawnsPerSide + 5] = makePiece<Bishop>(Color::black, {5, 7});
  m_pieces[0][k_pawnsPerSide + 6] = makePiece<Queen>(Color::black, {3, 7});
  m_pieces[0][k_pawnsPerSide + 7] = makePiece<King>(Color::black, {4, 7});

  m_pawnMovedOrPieceCaptured = false;
  m_fiftyMoveRuleCount = 0;
  m_castleStatus.set();
}

void Board::loadFromState(const LumpedBoardAndGameState &state) {
  m_pieces.clear();
  m_pieces.resize(2);

  // Wow, templated lambdas! C++20 is hot stuff
  auto createPiecesFromContainer =
      [this]<class T>(const PieceContainer &container) {
        int blackCounter = 0;
        int whiteCounter = 0;

        for (size_t i = 0; i < container.size(); ++i) {
          const auto piece = container[i];

          if (piece.first == Color::black) {
            m_pieces[0].emplace_back(makePiece<T>(piece.first, piece.second));
            ++blackCounter;
          } else if (piece.first == Color::white) {
            m_pieces[1].emplace_back(makePiece<T>(piece.first, piece.second));
            ++whiteCounter;
          }
        }
      };

  // I have to be real with you, though, this syntax is an abomination
  // Even Intellisense is mad about it
  createPiecesFromContainer.template operator()<Pawn>(state.pawns);
  createPiecesFromContainer.template operator()<Rook>(state.rooks);
  createPiecesFromContainer.template operator()<Knight>(state.knights);
  createPiecesFromContainer.template operator()<Bishop>(state.bishops);
  createPiecesFromContainer.template operator()<Queen>(state.queens);
  createPiecesFromContainer.template operator()<King>(state.kings);

  m_castleStatus = state.castleStatus;
  m_enPassantStatus = state.enPassantStatus;

  m_fiftyMoveRuleCount = state.halfMoveNum;

  refreshValidMoves();
}

void Board::cliDisplay(Color color) {
  auto innerLoop = [this](int i) {
    for (int j = 0; j < 8; ++j) {
      const auto *piece = getPieceAt({j, i});
      if (!piece) {
        std::cout << ".";
      } else {
        std::cout << piece->getLetter();
      }

      if (j == 7) {
        std::cout << std::endl;
      }
    }
  };

  if (color == Color::white) {
    for (int i = 7; i >= 0; --i) {
      innerLoop(i);
    }
  } else {
    for (int i = 0; i < 8; ++i) {
      innerLoop(i);
    }
  }
}

void Board::sdlDisplay(Color color) {
  // Handle displaying the board from the active player's perspective
  int verticalOffset = 0;

  if (color == Color::white) {
    verticalOffset = 7;
  }

  // Render board squares
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      ((i + j) % 2 == 0)
          ? sdlDrawSquare({std::abs(verticalOffset - j), i}, k_evenColor)
          : sdlDrawSquare({std::abs(verticalOffset - j), i}, k_oddColor);
    }
  }

  // Highlight piece that was clicked, if any
  if (m_pieceToHighlight.has_value()) {
    const auto &position = m_pieceToHighlight.value();
    sdlDrawSquare({position.first, std::abs(verticalOffset - position.second)},
                  k_selectedPieceColor);
  }

  // Highlight last black piece that moved
  if (m_blackLastMoveHighlight.has_value()) {
    const auto &position = m_blackLastMoveHighlight.value();
    sdlDrawSquare({position.first, std::abs(verticalOffset - position.second)},
                  k_blackLastMoveColor);
  }

  // Highlight last white piece that moved
  if (m_whiteLastMoveHighlight.has_value()) {
    const auto &position = m_whiteLastMoveHighlight.value();
    sdlDrawSquare({position.first, std::abs(verticalOffset - position.second)},
                  k_whiteLastMoveColor);
  }

  // Highlight valid moves for piece that was clicked, if any
  for (size_t i = 0; i < m_movesToHighlight.size(); ++i) {
    const auto &position = m_movesToHighlight[i];
    sdlDrawSquare({position.first, std::abs(verticalOffset - position.second)},
                  k_movementOptionColor);
  }

  // Highlights king's square with a warning color if in check
  if (m_kingToHighlight.has_value()) {
    const auto &position = m_kingToHighlight.value();
    sdlDrawSquare({position.first, std::abs(verticalOffset - position.second)},
                  k_checkColor);
  }

  // Render all pieces
  for (auto &side : m_pieces) {
    for (auto &piece : side) {
      sdlDrawPiece(color, piece.get(), verticalOffset);
    }
  }
}

void Board::sdlDrawSquare(const Position &position,
                          const SDL_Color &sdlColor) const {
  SDL_Rect square;
  square.w = k_squareWidth;
  square.h = k_squareHeight;
  SDL_SetRenderDrawColor(m_renderer, sdlColor.r, sdlColor.g, sdlColor.b,
                         sdlColor.a);

  square.x = position.first * k_squareWidth;
  square.y = position.second * k_squareHeight;
  SDL_RenderFillRect(m_renderer, &square);
}

void Board::sdlDrawPiece(Color color, const Piece *piece,
                         int verticalOffset) const {
  RETURN_IF_NULL(piece);

  int pieceXOffset = 0;
  int pieceYOffset = 0;
  int screenXOffset = 0;

  if (piece->getColor() == Color::black) {
    // Black is in top half of image
    pieceYOffset += k_pieceHeight;
  }

  if (dynamic_cast<const Pawn *>(piece)) {
    pieceXOffset += k_xPawnOffset;
  } else if (dynamic_cast<const Knight *>(piece)) {
    pieceXOffset += k_xKnightOffset;
  } else if (dynamic_cast<const Bishop *>(piece)) {
    pieceXOffset += k_xBishopOffset;
  } else if (dynamic_cast<const Rook *>(piece)) {
    pieceXOffset += k_xRookOffset;
  } else if (dynamic_cast<const Queen *>(piece)) {
    pieceXOffset += k_xQueenOffset;
  } else if (dynamic_cast<const King *>(piece)) {
    pieceXOffset += k_xKingOffset;
    screenXOffset += k_xKingAdditionalOffset;
  } else {
    // Type ???
    return;
  }

  const Position position = piece->getPosition();

  SDL_Rect pieceBox = {.x = pieceXOffset,
                       .y = pieceYOffset,
                       .w = k_pieceWidth,
                       .h = k_pieceHeight};

  SDL_Rect screenBox = {
      .x = (position.first * k_screenBoxSize) - k_screenBoxOffset +
           screenXOffset,
      .y = (std::abs(verticalOffset - position.second) * k_screenBoxSize) -
           k_screenBoxOffset,
      .w = k_pieceWidth,
      .h = k_pieceHeight};

  SDL_RenderCopy(m_renderer, m_pieceImageTexture, &pieceBox, &screenBox);
}

void Board::sdlDisplayPromotionOptions(Color color) const {
  int verticalOffset = 0;

  if (color == Color::white) {
    verticalOffset = 7;
  }

  // Draw board
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      ((i + j) % 2 == 0)
          ? sdlDrawSquare({std::abs(verticalOffset - j), i}, k_evenColor)
          : sdlDrawSquare({std::abs(verticalOffset - j), i}, k_oddColor);
    }
  }

  int allPiecesYOffset = 0;

  if (color == Color::black) {
    // Black is in top half of image
    allPiecesYOffset += k_pieceHeight;
  }

  std::array<SDL_Rect, 4> pieceBoxes = {SDL_Rect({.x = k_xKnightOffset,
                                                  .y = allPiecesYOffset,
                                                  .w = k_pieceWidth,
                                                  .h = k_pieceHeight}),
                                        SDL_Rect({.x = k_xBishopOffset,
                                                  .y = allPiecesYOffset,
                                                  .w = k_pieceWidth,
                                                  .h = k_pieceHeight}),
                                        SDL_Rect({.x = k_xRookOffset,
                                                  .y = allPiecesYOffset,
                                                  .w = k_pieceWidth,
                                                  .h = k_pieceHeight}),
                                        SDL_Rect({.x = k_xQueenOffset,
                                                  .y = allPiecesYOffset,
                                                  .w = k_pieceWidth,
                                                  .h = k_pieceHeight})};

  for (size_t i = 0; i < pieceBoxes.size(); ++i) {
    SDL_Rect screenBox = {
        .x = (static_cast<int>(k_xPromotionOffset + i) * k_screenBoxSize) -
             k_screenBoxOffset,
        .y = -k_screenBoxOffset,
        .w = k_pieceWidth,
        .h = k_pieceHeight};

    SDL_RenderCopy(m_renderer, m_pieceImageTexture, &pieceBoxes[i], &screenBox);
  }
}

Piece *Board::getPieceAt(const Position &position) {
  for (auto &side : m_pieces) {
    for (auto &piece : side) {
      CONTINUE_IF_NULL(piece);
      // Doesn't really feel optimal, but it does work
      if (piece->getPosition() == position) {
        return piece.get();
      }
    }
  }

  return nullptr;
}

std::pair<size_t, size_t> Board::getIndexOfPiece(const Piece *piece) {
  for (size_t i = 0; i < m_pieces.size(); ++i) {
    for (size_t j = 0; j < m_pieces[i].size(); ++j) {
      if (m_pieces[i][j].get() == piece) {
        return {i, j};
      }
    }
  }

  // Could use optional instead, but it's very overkill because we know
  // the piece we're searching for is in the container
  return {};
}

void Board::capturePiece(const Position &position) {
  m_pawnMovedOrPieceCaptured = true;

  for (auto &side : m_pieces) {
    for (auto &piece : side) {
      CONTINUE_IF_NULL(piece);
      if (piece->getPosition() == position) {
        piece.reset();
        piece = nullptr;
      }
    }
  }
}

bool Board::isValidMove(Color color, const Position &start, const Position &end,
                        const bool forMoveStorage) {
  auto *pieceToMove = getPieceAt(start);
  auto *pieceAtDestination = getPieceAt(end);

  // Nothing there
  if (!pieceToMove) {
    return false;
  }

  // Invalid position
  if (end.first >= 8 || end.second >= 8 || end.first < 0 || end.second < 0) {
    return false;
  }

  // Can't move pieces of the opposing color
  // Allow this for the computer player as they need to check advantages
  // for both players
  if (pieceToMove->getColor() != color && !m_isComputerPlaying) {
    return false;
  }

  // Can't capture pieces that are the same color
  if (pieceAtDestination) {
    if (pieceToMove->getColor() == pieceAtDestination->getColor()) {
      return false;
    }
  }

  // Store as local variables so these heavy functions don't get called often
  const bool inCheck = isKingInCheck(color);
  const bool willBeInCheck = moveAndCheckForCheck(color, start, end);

  // First see if the king is in check
  // Then see if king will be in check
  if (inCheck) {
    if (willBeInCheck) {
      return false;
    }
  }

  // If this move leads to check, it is illegal
  if (willBeInCheck) {
    return false;
  }

  // Castling case
  // Holy nested ifs, Batman!
  // TODO: Should be possible to clean this up
  if (dynamic_cast<King *>(pieceToMove)) {
    Position directionToMove = getDirectionVector(start, end);
    if (std::abs(directionToMove.first) == 2 && directionToMove.second == 0) {
      // Cannot castle if king has moved
      if (pieceToMove->hasMoved()) {
        return false;
      }

      // Cannot castle if in check
      if (inCheck) {
        return false;
      }

      // Check squares along intended path to see if they are attacked
      int direction = sign(directionToMove.first);
      // Queenside castle must check one additional square
      int queensideCastleExtension = (direction < 0) ? 1 : 0;
      for (int i = -1 - queensideCastleExtension; i < 1; ++i) {
        if (isSquareAttacked(color,
                             {end.first + (i * direction), end.second})) {
          return false;
        }
      }

      if (direction > 0 && color == Color::black) {
        // Black kingside castle
        if (m_castleStatus[k_blackKingsideIndex] != 0) {
          // Cannot castle if rook has moved
          if (dynamic_cast<Rook *>(getPieceAt({7, 7})) &&
              !getPieceAt({7, 7})->hasMoved()) {
            // Can only castle if no pieces are in the way
            if (!isPieceBlockingRook({7, 7}, {4, 7})) {
              if (!forMoveStorage) {
                // First make sure we can't castle again
                m_castleStatus[k_blackKingsideIndex] = 0;
                m_castleStatus[k_blackQueensideIndex] = 0;

                // Now move pieces
                movePiece({7, 7}, {5, 7});
                movePiece({4, 7}, {6, 7});
              }

              return true;
            }
          }
        }
      } else if (direction < 0 && color == Color::black) {
        // Black queenside castle
        if (m_castleStatus[k_blackQueensideIndex] != 0) {
          if (dynamic_cast<Rook *>(getPieceAt({0, 7})) &&
              !getPieceAt({0, 7})->hasMoved()) {
            if (!isPieceBlockingRook({0, 7}, {4, 7})) {
              if (!forMoveStorage) {
                m_castleStatus[k_blackKingsideIndex] = 0;
                m_castleStatus[k_blackQueensideIndex] = 0;

                movePiece({0, 7}, {3, 7});
                movePiece({4, 7}, {2, 7});
              }

              return true;
            }
          }
        }
      } else if (direction > 0 && color == Color::white) {
        // White kingside castle
        if (m_castleStatus[k_whiteKingsideIndex] != 0) {
          if (dynamic_cast<Rook *>(getPieceAt({7, 0})) &&
              !getPieceAt({7, 0})->hasMoved()) {
            if (!isPieceBlockingRook({7, 0}, {4, 0})) {
              if (!forMoveStorage) {
                m_castleStatus[k_whiteKingsideIndex] = 0;
                m_castleStatus[k_whiteQueensideIndex] = 0;

                movePiece({7, 0}, {5, 0});
                movePiece({4, 0}, {6, 0});
              }

              return true;
            }
          }
        }
      } else if (direction < 0 && color == Color::white) {
        // White queenside castle
        if (m_castleStatus[k_whiteQueensideIndex] != 0) {
          if (dynamic_cast<Rook *>(getPieceAt({0, 0})) &&
              !getPieceAt({0, 0})->hasMoved()) {
            if (!isPieceBlockingRook({0, 0}, {4, 0})) {
              if (!forMoveStorage) {
                m_castleStatus[k_whiteKingsideIndex] = 0;
                m_castleStatus[k_whiteQueensideIndex] = 0;

                movePiece({0, 0}, {3, 0});
                movePiece({4, 0}, {2, 0});
              }

              return true;
            }
          }
        }
      }
    }
  }

  // Side note: I kinda hate pawns as they are the only pieces in the game
  // whose move and attack squares are different, which leads to extra logic
  if (dynamic_cast<Pawn *>(pieceToMove)) {
    Position directionToMove = getDirectionVector(start, end);
    int sign = (pieceToMove->getColor() == Color::white) ? -1 : 1;
    // Pawns are allowed to move diagonally only if there's a piece to capture
    if (pieceAtDestination) {
      if (pieceToMove->getColor() != pieceAtDestination->getColor()) {
        if ((start.second == end.second + sign) &&
            ((start.first == end.first + 1) ||
             (start.first == end.first - 1))) {
          return true;
        }
      }
    } else {
      // En passant case
      // Pawn can capture only if there is a valid en passant square
      if (m_enPassantStatus.has_value()) {
        if (end == m_enPassantStatus->second &&
            color != m_enPassantStatus->first) {
          const int enPassantX = m_enPassantStatus->second.first;
          const int enPassantY = m_enPassantStatus->second.second;
          if ((pieceToMove->getPosition().first + 1) == enPassantX ||
              (pieceToMove->getPosition().first - 1) == enPassantX) {
            if (pieceToMove->getPosition().second - sign == enPassantY) {
              if (!forMoveStorage) {
                // Handle capture here as it is unorthodox
                movePiece(start, end);
                capturePiece({end.first, end.second + sign});
              }

              return true;
            }
          }
        }
      }
    }

    // Pawns cannot capture if moving normally
    // and cannot move through pieces when moving 2 squares
    if (pieceAtDestination) {
      return false;
    } else if (std::abs(directionToMove.second) == 2) {
      if (pieceAtDestination || getPieceAt({end.first, end.second + sign})) {
        return false;
      }
    }
  }

  // Most obvious case - check if requested move is in agreement with piece
  // logic
  if (!pieceToMove->isValidMove(end)) {
    return false;
  }

  // Check for pieces blocking path
  if (dynamic_cast<Bishop *>(pieceToMove)) {
    if (isPieceBlockingBishop(start, end)) {
      return false;
    }
  } else if (dynamic_cast<Rook *>(pieceToMove)) {
    if (isPieceBlockingRook(start, end)) {
      return false;
    }
  } else if (dynamic_cast<Queen *>(pieceToMove)) {
    Position directionVector = getDirectionVector(start, end);
    if (std::abs(directionVector.first) == std::abs(directionVector.second)) {
      if (isPieceBlockingBishop(start, end)) {
        return false;
      }
    } else {
      if (isPieceBlockingRook(start, end)) {
        return false;
      }
    }
  }

  return true;
}

void Board::movePiece(const Position &start, const Position &end) {
  auto *pieceToMove = getPieceAt(start);
  auto *pieceAtDestination = getPieceAt(end);

  // This helps in a surprising number of ways
  RETURN_IF_NULL(pieceToMove);

  // Update flag tracking if piece has moved yet
  if (!pieceToMove->hasMoved()) {
    pieceToMove->setHasMoved(true);
  }

  // All cases should be checked at this point
  // Capturing correctly is assumed
  if (pieceAtDestination) {
    capturePiece(end);
    pieceToMove->setPosition(end);
  } else {
    pieceToMove->setPosition(end);
  }
}

bool Board::isPieceBlockingBishop(const Position &start, const Position &end) {
  Position directionVector = getDirectionVector(start, end);
  // Direction vector components will be equal other than sign
  int magnitude = std::abs(directionVector.first);
  Piece *potentialPiece;

  for (int i = 1; i <= magnitude; ++i) {
    potentialPiece =
        getPieceAt({start.first + (i * sign(directionVector.first)),
                    start.second + (i * sign(directionVector.second))});

    // Piece found
    if (potentialPiece && potentialPiece->getPosition() != end) {
      return true;
    }
  }

  return false;
}

bool Board::isPieceBlockingRook(const Position &start, const Position &end) {
  Position directionVector = getDirectionVector(start, end);
  // One component of vector will be zero
  int magnitude = 0;
  bool movingVertically = false;
  Piece *potentialPiece = nullptr;

  if (directionVector.first == 0) {
    magnitude = std::abs(directionVector.second);
    movingVertically = true;
  } else if (directionVector.second == 0) {
    magnitude = std::abs(directionVector.first);
    movingVertically = false;
  }

  for (int i = 1; i <= magnitude; ++i) {
    if (movingVertically) {
      potentialPiece = getPieceAt(
          {start.first, start.second + (i * sign(directionVector.second))});
    } else {
      potentialPiece = getPieceAt(
          {start.first + (i * sign(directionVector.first)), start.second});
    }

    // Piece found
    if (potentialPiece && potentialPiece->getPosition() != end) {
      return true;
    }
  }

  return false;
}

bool Board::isSquareAttacked(Color color, const Position &position) {
  Position positionToCheck;
  // Rook case
  // Should be a way to refactor/combine these...
  for (int i = 0; i < 8; ++i) {
    positionToCheck = {position.first, i};
    const auto *potentialAttackerX = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerX);

    // Don't check own position
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<const Rook *>(potentialAttackerX) ||
        dynamic_cast<const Queen *>(potentialAttackerX)) {
      // Don't count own pieces
      CONTINUE_IF_VALID(potentialAttackerX->getColor() == color);

      if (!isPieceBlockingRook(potentialAttackerX->getPosition(), position)) {
        return true;
      }
    }
  }

  for (int i = 0; i < 8; ++i) {
    positionToCheck = {i, position.second};
    const auto *potentialAttackerY = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerY);

    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<const Rook *>(potentialAttackerY) ||
        dynamic_cast<const Queen *>(potentialAttackerY)) {
      CONTINUE_IF_VALID(potentialAttackerY->getColor() == color);

      if (!isPieceBlockingRook(potentialAttackerY->getPosition(), position)) {
        return true;
      }
    }
  }

  // Bishop case
  // The case for refactoring the rook case doubly applies here
  for (int i = 1; i < 8; ++i) {
    if (position.first + i > 7 || position.second + i > 7) {
      continue;
    }
    positionToCheck = {position.first + i, position.second + i};

    const auto *potentialAttackerNortheast = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerNortheast);

    CONTINUE_IF_VALID(potentialAttackerNortheast->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<const Bishop *>(potentialAttackerNortheast) ||
        dynamic_cast<const Queen *>(potentialAttackerNortheast)) {
      if (!isPieceBlockingBishop(potentialAttackerNortheast->getPosition(),
                                 position)) {
        { return true; }
      }
    }
  }

  for (int i = 1; i < 8; ++i) {
    if (position.first - i < 0 || position.second + i > 7) {
      continue;
    }
    positionToCheck = {position.first - i, position.second + i};

    const auto *potentialAttackerNorthwest = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerNorthwest);

    CONTINUE_IF_VALID(potentialAttackerNorthwest->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<const Bishop *>(potentialAttackerNorthwest) ||
        dynamic_cast<const Queen *>(potentialAttackerNorthwest)) {
      if (!isPieceBlockingBishop(potentialAttackerNorthwest->getPosition(),
                                 position)) {
        { return true; }
      }
    }
  }

  for (int i = 1; i < 8; ++i) {
    if (position.first + i > 7 || position.second - i < 0) {
      continue;
    }
    positionToCheck = {position.first + i, position.second - i};

    const auto *potentialAttackerSoutheast = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerSoutheast);

    CONTINUE_IF_VALID(potentialAttackerSoutheast->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<const Bishop *>(potentialAttackerSoutheast) ||
        dynamic_cast<const Queen *>(potentialAttackerSoutheast)) {
      if (!isPieceBlockingBishop(potentialAttackerSoutheast->getPosition(),
                                 position)) {
        { return true; }
      }
    }
  }

  for (int i = 1; i < 8; ++i) {
    if (position.first - i < 0 || position.second - i < 0) {
      continue;
    }
    positionToCheck = {position.first - i, position.second - i};

    const auto *potentialAttackerSouthwest = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerSouthwest);

    CONTINUE_IF_VALID(potentialAttackerSouthwest->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<const Bishop *>(potentialAttackerSouthwest) ||
        dynamic_cast<const Queen *>(potentialAttackerSouthwest)) {
      if (!isPieceBlockingBishop(potentialAttackerSouthwest->getPosition(),
                                 position)) {
        { return true; }
      }
    }
  }

  // Knight case
  for (int i = 0; i < k_potentialKnightPositions.size(); ++i) {
    positionToCheck = {position.first + k_potentialKnightPositions[i].first,
                       position.second + k_potentialKnightPositions[i].second};

    const auto *potentialKnightAttacker = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialKnightAttacker);

    CONTINUE_IF_VALID(potentialKnightAttacker->getColor() == color);

    if (dynamic_cast<const Knight *>(potentialKnightAttacker)) {
      return true;
    }
  }

  // Pawn case
  {
    int sign = (color == Color::white) ? 1 : -1;

    const auto *potentialAttackerRight =
        getPieceAt({position.first + 1, position.second + sign});
    const auto *potentialAttackerLeft =
        getPieceAt({position.first - 1, position.second + sign});

    if ((dynamic_cast<const Pawn *>(potentialAttackerRight) &&
         (potentialAttackerRight->getColor() != color)) ||
        (dynamic_cast<const Pawn *>(potentialAttackerLeft) &&
         (potentialAttackerLeft->getColor() != color))) {
      return true;
    }
  }

  // King case
  {
    for (int i = -1; i < 2; ++i) {
      const auto *potentialAttackerLeft =
          getPieceAt({position.first - 1, position.second + i});
      const auto *potentialAttackerCenter =
          getPieceAt({position.first, position.second + i});
      const auto *potentialAttackerRight =
          getPieceAt({position.first + 1, position.second + i});

      if ((dynamic_cast<const King *>(potentialAttackerLeft) &&
           (potentialAttackerLeft->getColor() != color)) ||
          (dynamic_cast<const King *>(potentialAttackerCenter) &&
           (potentialAttackerCenter->getColor() != color)) ||
          (dynamic_cast<const King *>(potentialAttackerRight) &&
           (potentialAttackerRight->getColor() != color))) {
        return true;
      }
    }
  }

  return false;
}

bool Board::isKingInCheck(Color color) {
  const auto *king = getKingFromColor(color);

  if (!king) {
    // Should never happen
    return false;
  }

  return isSquareAttacked(color, king->getPosition());
}

bool Board::canKingGetOutOfCheck(Color color, const Position &start,
                                 const Position &end) {
  if (dynamic_cast<const King *>(getPieceAt(start))) {
    return isSquareAttacked(color, end);
  }

  return moveAndCheckForCheck(color, start, end);
}

bool Board::moveAndCheckForCheck(Color color, const Position &start,
                                 const Position &end) {
  auto *pieceToMove = getPieceAt(start);
  auto *pieceAtDestination = getPieceAt(end);

  if (!pieceToMove) {
    return false;
  }

  if (pieceAtDestination) {
    if (pieceToMove->getColor() == pieceAtDestination->getColor()) {
      return false;
    }
  }

  testMove(start, end);
  bool result = isKingInCheck(color);
  undoMove(start, end);

  return result;
}

void Board::storeValidMoves() {
  if (k_verbose) {
    std::cout << "Legal moves for each piece:" << std::endl;
  }

  // Initialize container to hold moves before they are added
  std::vector<FullMove> moveStorage;

  for (auto &side : m_pieces) {
    for (auto &piece : side) {
      moveStorage.clear();

      auto *pieceToCheck = piece.get();

      CONTINUE_IF_NULL(pieceToCheck);

      const auto &color = pieceToCheck->getColor();
      const auto &position = pieceToCheck->getPosition();

      // Guard for positions out of bounds
      if (position.first < 0 || position.second < 0 || position.first >= 8 ||
          position.second >= 8) {
        continue;
      }

      if (k_verbose) {
        std::cout << std::endl;
        std::cout << pieceToCheck->getLetter() << std::endl;
        std::cout << position.first << " " << position.second << std::endl;
      }

      // Check piece type and add all possible moves to storage
      // The point of all this is to optimize the time to check moves.
      // By preselecting the only moves possible before checking if the moves
      // are valid, there are less moves total to go through
      if (dynamic_cast<Pawn *>(pieceToCheck)) {
        int sign = (color == Color::white) ? 1 : -1;

        // Pawns have 4 potential moves maximum
        // and they also have en passant, if applicable
        moveStorage.emplace_back(
            PieceType::pawn, color, position,
            Position({position.first, position.second + (sign * 2)}));
        moveStorage.emplace_back(
            PieceType::pawn, color, position,
            Position({position.first, position.second + sign}));
        moveStorage.emplace_back(
            PieceType::pawn, color, position,
            Position({position.first - 1, position.second + sign}));
        moveStorage.emplace_back(
            PieceType::pawn, color, position,
            Position({position.first + 1, position.second + sign}));
        if (m_enPassantStatus.has_value()) {
          moveStorage.emplace_back(PieceType::pawn, color, position,
                                   m_enPassantStatus.value().second);
        }

      } else if (dynamic_cast<Knight *>(pieceToCheck)) {
        // Knights have 8 potential moves maximum
        for (int i = 0; i < k_potentialKnightPositions.size(); ++i) {
          moveStorage.emplace_back(
              PieceType::knight, color, position,
              Position(
                  {position.first + k_potentialKnightPositions[i].first,
                   position.second + k_potentialKnightPositions[i].second}));
        }
      } else if (dynamic_cast<Bishop *>(pieceToCheck)) {
        // Bishops have... well, now we have to check everything
        // TODO: optimize
        for (int i = 1; i < 8; ++i) {
          moveStorage.emplace_back(
              PieceType::bishop, color, position,
              Position({position.first + i, position.second + i}));
          moveStorage.emplace_back(
              PieceType::bishop, color, position,
              Position({position.first - i, position.second + i}));
          moveStorage.emplace_back(
              PieceType::bishop, color, position,
              Position({position.first - i, position.second - i}));
          moveStorage.emplace_back(
              PieceType::bishop, color, position,
              Position({position.first + i, position.second - i}));
        }
      } else if (dynamic_cast<Rook *>(pieceToCheck)) {
        for (int i = 1; i < 8; ++i) {
          moveStorage.emplace_back(
              PieceType::rook, color, position,
              Position({position.first + i, position.second}));
          moveStorage.emplace_back(
              PieceType::rook, color, position,
              Position({position.first, position.second + i}));
          moveStorage.emplace_back(
              PieceType::rook, color, position,
              Position({position.first - i, position.second}));
          moveStorage.emplace_back(
              PieceType::rook, color, position,
              Position({position.first, position.second - i}));
        }
      } else if (dynamic_cast<Queen *>(pieceToCheck)) {
        // Bishop/rook case combined as always
        for (int i = 1; i < 8; ++i) {
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first + i, position.second + i}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first - i, position.second + i}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first - i, position.second - i}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first + i, position.second - i}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first + i, position.second}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first, position.second + i}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first - i, position.second}));
          moveStorage.emplace_back(
              PieceType::queen, color, position,
              Position({position.first, position.second - i}));
        }
      } else if (dynamic_cast<King *>(pieceToCheck)) {
        // Kings have 8 potential moves maximum, except they can also castle
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first + 1, position.second + 1}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first - 1, position.second + 1}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first - 1, position.second - 1}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first + 1, position.second - 1}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first + 1, position.second}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first, position.second + 1}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first - 1, position.second}));
        moveStorage.emplace_back(
            PieceType::king, color, position,
            Position({position.first, position.second - 1}));

        if (color == Color::black) {
          if (m_castleStatus[k_blackKingsideIndex]) {
            moveStorage.emplace_back(
                PieceType::king, color, position,
                Position({position.first + 2, position.second}));
          }
          if (m_castleStatus[k_blackQueensideIndex]) {
            moveStorage.emplace_back(
                PieceType::king, color, position,
                Position({position.first - 2, position.second}));
          }
        } else {
          if (m_castleStatus[k_whiteKingsideIndex]) {
            moveStorage.emplace_back(
                PieceType::king, color, position,
                Position({position.first + 2, position.second}));
          }
          if (m_castleStatus[k_whiteQueensideIndex]) {
            moveStorage.emplace_back(
                PieceType::king, color, position,
                Position({position.first - 2, position.second}));
          }
        }
      }

      // See if stored moves are actually valid, and if they are,
      // add them to both the board's and piece's valid moves
      for (size_t i = 0; i < moveStorage.size(); ++i) {
        const auto &potentialMove = moveStorage[i];
        if (isValidMove(color, position, potentialMove.end, true)) {
          pieceToCheck->addValidMove(potentialMove.end);
          m_allValidMoves.emplace_back(potentialMove);

          if (k_verbose) {
            std::cout << potentialMove.end.first << " "
                      << potentialMove.end.second << std::endl;
          }
        }
      }
    }
  }
}

bool Board::isKingCheckmated(Color color) {
  auto checkForColor = [this, color](FullMove input) {
    return color == input.color;
  };

  if (isKingInCheck(color)) {
    if (std::find_if(m_allValidMoves.begin(), m_allValidMoves.end(),
                     checkForColor) == m_allValidMoves.end()) {
      // Highlight now because render stops being called after game over
      highlightKingInCheck(color);
      return true;
    }
  }

  return false;
}

bool Board::hasStalemateOccurred(Color color) {
  // 50-move rule
  if (m_pawnMovedOrPieceCaptured) {
    m_fiftyMoveRuleCount = 0;
  } else {
    ++m_fiftyMoveRuleCount;
  }

  m_pawnMovedOrPieceCaptured = false;

  if (m_fiftyMoveRuleCount >= 50) {
    return true;
  }

  // Dead positions
  if (checkForDeadPosition()) {
    return true;
  }

  // Identical to checkmate function, but removes the requirement of the king
  // being in check
  auto checkForColor = [this, color](FullMove input) {
    return color == input.color;
  };

  if (std::find_if(m_allValidMoves.begin(), m_allValidMoves.end(),
                   checkForColor) == m_allValidMoves.end()) {
    return true;
  }

  return false;
}

bool Board::promotePawn(const PieceType &piece) {
  if (!m_pawnToPromote.has_value()) {
    // Should never happen
    return false;
  }

  const Position position = m_pawnToPromote.value();
  auto *pawnToPromote = getPieceAt(position);

  // Figure out where in container the pawn is so we can reset the pointer
  const auto &index = getIndexOfPiece(pawnToPromote);

  // Store the color of the pawn
  Color color = position.second == 7 ? Color::white : Color::black;

  // Reset pointer
  m_pieces[index.first][index.second].reset();

  if (piece == PieceType::knight) {
    m_pieces[index.first][index.second] = makePiece<Knight>(color, position);
  } else if (piece == PieceType::bishop) {
    m_pieces[index.first][index.second] = makePiece<Bishop>(color, position);
  } else if (piece == PieceType::rook) {
    m_pieces[index.first][index.second] = makePiece<Rook>(color, position);
  } else if (piece == PieceType::queen) {
    m_pieces[index.first][index.second] = makePiece<Queen>(color, position);
  }

  m_pawnToPromote.reset();

  return true;
}

bool Board::promotePawn(const Position &positionToPromoteTo) {
  if (!m_pawnToPromote.has_value()) {
    // Should never happen
    return false;
  }

  const Position position = m_pawnToPromote.value();
  auto *pawnToPromote = getPieceAt(position);

  // Figure out where in container the pawn is so we can reset the pointer
  const auto &index = getIndexOfPiece(pawnToPromote);

  int verticalOffset = 0;

  if (pawnToPromote->getColor() == Color::black) {
    verticalOffset = 7;
  }

  if (positionToPromoteTo == Position({2, verticalOffset})) {
    promotePawn(PieceType::knight);
  } else if (positionToPromoteTo == Position({3, verticalOffset})) {
    promotePawn(PieceType::bishop);
  } else if (positionToPromoteTo == Position({4, verticalOffset})) {
    promotePawn(PieceType::rook);
  } else if (positionToPromoteTo == Position({5, verticalOffset})) {
    promotePawn(PieceType::queen);
  } else {
    return false;
  }

  return true;
}

void Board::updateBoardState(const Position &start, const Position &end) {
  // Clear en passant each turn
  m_enPassantStatus.reset();

  const auto *pieceThatMoved = getPieceAt(end);

  RETURN_IF_NULL(pieceThatMoved);

  Color color = pieceThatMoved->getColor();

  // Clear last move highlight for that color and set to the piece's new position
  if (color == Color::black) {
    m_blackLastMoveHighlight = pieceThatMoved->getPosition();
  } else {
    m_whiteLastMoveHighlight = pieceThatMoved->getPosition();
  }

  // Handle special events
  if (dynamic_cast<const Pawn *>(pieceThatMoved)) {
    m_pawnMovedOrPieceCaptured = true;

    int lastRow = (color == Color::white) ? 7 : 0;

    // Pawn moved two spaces - need to store en passant square
    if (std::abs(getDirectionVector(start, end).second) == 2) {
      int sign = (color == Color::white) ? -1 : 1;
      // Register en passant square as square directly behind pawn
      m_enPassantStatus = {color, {end.first, end.second + sign}};
    } else if (pieceThatMoved->getPosition().second == lastRow) {
      // Promotion case
      m_pawnToPromote = pieceThatMoved->getPosition();
    }
  } else if (dynamic_cast<const King *>(pieceThatMoved)) {
    // Update castle status
    if (color == Color::white) {
      m_castleStatus[k_whiteKingsideIndex] = 0;
      m_castleStatus[k_whiteQueensideIndex] = 0;
    } else {
      m_castleStatus[k_blackKingsideIndex] = 0;
      m_castleStatus[k_blackQueensideIndex] = 0;
    }
  } else if (dynamic_cast<const Rook *>(pieceThatMoved)) {
    if (color == Color::white && start == Position({0, 0})) {
      m_castleStatus[k_whiteQueensideIndex] = 0;
    } else if (color == Color::white && start == Position({7, 0})) {
      m_castleStatus[k_whiteQueensideIndex] = 0;
    } else if (color == Color::black && start == Position({0, 7})) {
      m_castleStatus[k_blackQueensideIndex] = 0;
    } else if (color == Color::black && start == Position({7, 7})) {
      m_castleStatus[k_blackKingsideIndex] = 0;
    }
  }

  // Clear storage for moves to highlight
  clearOldPieceHighlight();
  clearOldKingHighlight();
}

bool Board::isInputValid(Color color, const std::queue<Position> &positions) {
  // Can't move nothing
  if (!getPieceAt(positions.front())) {
    return false;
  }

  // Can't move opponent's pieces
  if (getPieceAt(positions.front())->getColor() != color) {
    return false;
  }

  // Can't move if destination is our own piece
  if (positions.size() > 1 && getPieceAt(positions.back())) {
    if (getPieceAt(positions.back())->getColor() == color) {
      return false;
    }
  }

  return true;
}

void Board::highlightPotentialMoves(const Position &position) {
  // Clear storage containers first
  clearOldPieceHighlight();

  const auto *pieceToHighlight = getPieceAt(position);

  RETURN_IF_NULL(pieceToHighlight);

  m_pieceToHighlight = pieceToHighlight->getPosition();

  const auto &validMoves = pieceToHighlight->getValidMoves();

  if (validMoves.size() < 1) {
    // No possible moves
    return;
  }

  for (size_t i = 0; i < validMoves.size(); ++i) {
    m_movesToHighlight.emplace_back(validMoves[i]);
  }
}

void Board::highlightKingInCheck(Color color) {
  const auto *king = getKingFromColor(color);

  if (!king) {
    // Should never happen
    return;
  }

  m_kingToHighlight = king->getPosition();
}

const std::vector<FullMove> Board::getValidMovesFor(Color color) const {
  std::vector<FullMove> toReturn = {};
  for (const auto &move : m_allValidMoves) {
    if (color == move.color) {
      toReturn.emplace_back(move);
    }
  }

  return toReturn;
}

void Board::refreshValidMoves() {
  m_allValidMoves.clear();
  for (auto &side : m_pieces) {
    for (auto &piece : side) {
      CONTINUE_IF_NULL(piece);
      piece->clearValidMoves();
    }
  }

  storeValidMoves();
}

const LumpedBoardAndGameState &
Board::getBoardAndGameState(Color color, size_t halfMoveNum, size_t turnNum) {
  m_boardAndGameState = {};

  for (const auto &side : m_pieces) {
    for (const auto &piece : side) {
      const auto *pieceToStore = piece.get();
      CONTINUE_IF_NULL(pieceToStore);

      const auto &color = pieceToStore->getColor();
      const auto &position = pieceToStore->getPosition();

      if (dynamic_cast<const Pawn *>(pieceToStore)) {
        m_boardAndGameState.pawns.emplace_back(color, position);
      } else if (dynamic_cast<const Knight *>(pieceToStore)) {
        m_boardAndGameState.knights.emplace_back(color, position);
      } else if (dynamic_cast<const Bishop *>(pieceToStore)) {
        m_boardAndGameState.bishops.emplace_back(color, position);
      } else if (dynamic_cast<const Rook *>(pieceToStore)) {
        m_boardAndGameState.rooks.emplace_back(color, position);
      } else if (dynamic_cast<const Queen *>(pieceToStore)) {
        m_boardAndGameState.queens.emplace_back(color, position);
      } else if (dynamic_cast<const King *>(pieceToStore)) {
        m_boardAndGameState.kings.emplace_back(color, position);
      }
    }
  }

  m_boardAndGameState.castleStatus = m_castleStatus;
  m_boardAndGameState.enPassantStatus = m_enPassantStatus;

  // Function arguments are passed from game instance
  m_boardAndGameState.whoseTurn = color;
  m_boardAndGameState.halfMoveNum = halfMoveNum;
  m_boardAndGameState.turnNum = turnNum;

  return m_boardAndGameState;
}

const Piece *Board::getKingFromColor(Color color) const {
  Piece *king = nullptr;

  for (const auto &side : m_pieces) {
    for (const auto &piece : side) {
      CONTINUE_IF_NULL(piece);

      if (dynamic_cast<King *>(piece.get())) {
        if (piece->getColor() == color) {
          king = piece.get();
        }
      }
    }
  }

  return king;
}

void Board::testMove(const Position &start, const Position &end, int depth) {
  auto *pieceToMove = getPieceAt(start);
  auto *pieceAtDestination = getPieceAt(end);

  RETURN_IF_NULL(pieceToMove);

  if (pieceAtDestination) {
    if (pieceToMove->getColor() == pieceAtDestination->getColor()) {
      return;
    }
    // Banish to the shadow realm... for now
    pieceAtDestination->setPosition({-depth, -depth});
    pieceToMove->setPosition(end);
  }

  pieceToMove->setPosition(end);
}

void Board::undoMove(const Position &start, const Position &end, int depth) {
  auto *pieceToMove = getPieceAt(end);
  auto *pieceAtDestination = getPieceAt({-depth, -depth});

  RETURN_IF_NULL(pieceToMove);

  if (pieceAtDestination) {
    // You're finally awake...
    pieceAtDestination->setPosition(end);
  }

  pieceToMove->setPosition(start);
}

bool Board::checkForDeadPosition() const {
  auto onlyKingLeft = [this](Color color) {
    int sideIndex = 0;

    if (color == Color::white) {
      sideIndex = 1;
    }

    if (m_pieces[sideIndex].size() == 1) {
      if (static_cast<King *>(m_pieces[sideIndex].front().get())) {
        return true;
      }
    }

    return false;
  };

  auto kingAndOnePieceLeft = [this]<class T>(Color color) {
    int sideIndex = 0;

    if (color == Color::white) {
      sideIndex = 1;
    }

    if (m_pieces[sideIndex].size() == 2) {
      if ((static_cast<King *>(m_pieces[sideIndex].front().get()) &&
           static_cast<T *>(m_pieces[sideIndex].back().get())) ||
          (static_cast<T *>(m_pieces[sideIndex].front().get()) &&
           static_cast<King *>(m_pieces[sideIndex].back().get()))) {
        return true;
      }
    }

    return false;
  };

  auto areBishopsOnSameColor = [this]() {
    Position whiteBishopStartingPosition = {};
    Position blackBishopStartingPosition = {};

    for (const auto &side : m_pieces) {
      for (const auto &piece : side) {
        CONTINUE_IF_NULL(piece);

        if (dynamic_cast<const Bishop *>(piece.get()) &&
            piece->getColor() == Color::white) {
          whiteBishopStartingPosition = piece->getStartingPosition();
        } else if (dynamic_cast<const Bishop *>(piece.get()) &&
                   piece->getColor() == Color::black) {
          blackBishopStartingPosition = piece->getStartingPosition();
        }
      }
    }

    if ((whiteBishopStartingPosition.first % 2 == 0 &&
         blackBishopStartingPosition.first % 2 != 0) ||
        (whiteBishopStartingPosition.first % 2 != 0 &&
         blackBishopStartingPosition.first % 2 == 0)) {
      return true;
    }

    return false;
  };

  // First, single king cases
  if (onlyKingLeft(Color::black) && onlyKingLeft(Color::white)) {
    return true;
  }

  // Now cases with multiple pieces
  if ((kingAndOnePieceLeft.template operator()<Bishop>(Color::white) &&
       onlyKingLeft(Color::black)) ||
      (kingAndOnePieceLeft.template operator()<Bishop>(Color::black) &&
       onlyKingLeft(Color::white))) {
    return true;
  } else if ((kingAndOnePieceLeft.template operator()<Knight>(Color::white) &&
              onlyKingLeft(Color::black)) ||
             (kingAndOnePieceLeft.template operator()<Knight>(Color::black) &&
              onlyKingLeft(Color::white))) {
    return true;
  } else if (kingAndOnePieceLeft.template operator()<Bishop>(Color::white) &&
             kingAndOnePieceLeft.template operator()<Bishop>(Color::black)) {
    // Only true if bishops are on the same color
    return areBishopsOnSameColor();
  } else if (kingAndOnePieceLeft.template operator()<Bishop>(Color::black) &&
             kingAndOnePieceLeft.template operator()<Bishop>(Color::white)) {
    // Only true if bishops are on the same color
    return areBishopsOnSameColor();
  }

  return false;
}
