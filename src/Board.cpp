#include "Board.h"

namespace {

const std::string k_pieceImageFilepath = "../chess/inc/pieces.png";

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
constexpr SDL_Color k_selectedPieceColor = SDL_Color({255, 255, 0, 128});;

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
}

void Board::loadFromFen(const BoardLayout &layout) {
  // Wow, templated lambdas! C++20 is hot stuff
  auto createPiecesFromContainer =
      [this]<class T>(const PieceContainer &container, int offset) {
        int blackCounter = 0;
        int whiteCounter = 0;

        for (size_t i = 0; i < container.size(); ++i) {
          const auto piece = container[i];

          if (piece.first == Color::black) {
            m_pieces[0][blackCounter + offset] =
                makePiece<T>(piece.first, piece.second);
            ++blackCounter;
          } else if (piece.first == Color::white) {
            m_pieces[1][whiteCounter + offset] =
                makePiece<T>(piece.first, piece.second);
            ++whiteCounter;
          }
        }
      };

  // I have to be real with you, though, this syntax is an abomination
  // Even Intellisense is mad about it
  createPiecesFromContainer.template operator()<Pawn>(layout.pawns, 0);
  createPiecesFromContainer.template operator()<Rook>(layout.rooks,
                                                      k_pawnsPerSide);
  createPiecesFromContainer.template operator()<Knight>(layout.knights,
                                                        k_pawnsPerSide + 2);
  createPiecesFromContainer.template operator()<Bishop>(layout.bishops,
                                                        k_pawnsPerSide + 4);
  createPiecesFromContainer.template operator()<Queen>(layout.queens,
                                                       k_pawnsPerSide + 6);
  createPiecesFromContainer.template operator()<King>(layout.kings,
                                                      k_pawnsPerSide + 7);

  m_castleStatus = layout.castleStatus;
  m_enPassantSquare = layout.enPassantTarget;
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

void Board::sdlDisplay() {
  // Render board squares
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      ((i + j) % 2 == 0) ? sdlDrawSquare({j, i}, k_evenColor)
                         : sdlDrawSquare({j, i}, k_oddColor);
    }
  }

  // Highlight piece that was clicked, if any
  if (m_pieceToHighlight.has_value()) {
    sdlDrawSquare(m_pieceToHighlight.value(), k_selectedPieceColor);
  }

  // Highlight valid moves for piece that was clicked, if any
  for (size_t i = 0; i < m_movesToHighlight.size(); ++i) {
    sdlDrawSquare(m_movesToHighlight.at(i).second, k_movementOptionColor);
  }

  // Highlights king's square with a warning color if in check
  if (m_kingToHighlight.has_value()) {
    sdlDrawSquare(m_kingToHighlight.value(), k_checkColor);
  }

  // Render all pieces
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < k_totalPieces / 2; ++j) {
      sdlDrawPiece(m_pieces[i][j].get());
    }
  }
}

void Board::sdlDrawSquare(const Position &position, const SDL_Color &sdlColor) {
  SDL_Rect square;
  square.w = k_squareWidth;
  square.h = k_squareHeight;
  SDL_SetRenderDrawColor(m_renderer, sdlColor.r, sdlColor.g, sdlColor.b,
                         sdlColor.a);

  square.x = position.first * k_squareWidth;
  square.y = position.second * k_squareHeight;
  SDL_RenderFillRect(m_renderer, &square);
}

void Board::sdlDrawPiece(const Piece *piece) {
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

  SDL_Rect screenBox = {.x = (position.first * k_screenBoxSize) -
                             k_screenBoxOffset + screenXOffset,
                        .y = (position.second * k_screenBoxSize) -
                             k_screenBoxOffset,
                        .w = k_pieceWidth,
                        .h = k_pieceHeight};

  SDL_RenderCopy(m_renderer, m_pieceImageTexture, &pieceBox, &screenBox);
}

Piece *Board::getPieceAt(const Position &position) {
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < k_totalPieces / 2; ++j) {
      CONTINUE_IF_NULL(m_pieces[i][j]);
      // Doesn't really feel optimal, but it does work
      if (m_pieces[i][j]->getPosition() == position) {
        return m_pieces[i][j].get();
      }
    }
  }

  return nullptr;
}

std::pair<size_t, size_t> Board::getIndexOfPiece(const Piece *piece) {
  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < k_totalPieces / 2; ++j) {
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
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < k_totalPieces / 2; ++j) {
      CONTINUE_IF_NULL(m_pieces[i][j]);
      if (m_pieces[i][j]->getPosition() == position) {
        m_pieces[i][j].reset();
        m_pieces[i][j] = nullptr;
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

  // Can't move pieces of the opposing color
  if (pieceToMove->getColor() != color) {
    return false;
  }

  // Cannot capture pieces that are the same color
  if (pieceAtDestination) {
    if (pieceToMove->getColor() == pieceAtDestination->getColor()) {
      return false;
    }
  }

  // First see if the king is in check
  // Then see if king will be in check
  if (isKingInCheck(color)) {
    if (moveAndCheckForCheck(color, start, end)) {
      return false;
    }
  }

  // If this move leads to check, it is illegal
  if (moveAndCheckForCheck(color, start, end)) {
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

      // Check squares along intended path to see if they are attacked
      int direction = sign(directionToMove.first);
      for (int i = 1; i < 3; ++i) {
        if (isSquareAttacked(color, {directionToMove.first + (i * direction),
                                     directionToMove.second})) {
          return false;
        }
      }

      if (sign(directionToMove.first) > 0 && color == Color::black) {
        // Black kingside castle
        if (m_castleStatus[k_blackKingsideIndex] != 0) {
          // Cannot castle if rook has moved
          if (dynamic_cast<Rook *>(getPieceAt({7, 7})) &&
              !getPieceAt({7, 7})->hasMoved()) {
            // Can only castle if no pieces are in the way
            if (!isPieceBlockingRook({7, 7}, {4, 7})) {
              if (!forMoveStorage) {
                // First make sure we can't castle again
                m_castleStatus.reset();
                // Now move pieces
                movePiece({7, 7}, {5, 7});
                movePiece({4, 7}, {6, 7});
              }

              return true;
            }
          }
        }
      } else if (sign(directionToMove.first) < 0 && color == Color::black) {
        // Black queenside castle
        if (m_castleStatus[k_blackQueensideIndex] != 0) {
          if (dynamic_cast<Rook *>(getPieceAt({0, 7})) &&
              !getPieceAt({0, 7})->hasMoved()) {
            if (!isPieceBlockingRook({0, 7}, {4, 7})) {
              if (!forMoveStorage) {
                m_castleStatus.reset();

                movePiece({0, 7}, {3, 7});
                movePiece({4, 7}, {2, 7});
              }

              return true;
            }
          }
        }
      } else if (sign(directionToMove.first) > 0 && color == Color::white) {
        // White kingside castle
        if (m_castleStatus[k_whiteKingsideIndex] != 0) {
          if (dynamic_cast<Rook *>(getPieceAt({7, 0})) &&
              !getPieceAt({7, 0})->hasMoved()) {
            if (!isPieceBlockingRook({7, 0}, {4, 0})) {
              if (!forMoveStorage) {
                m_castleStatus.reset();

                movePiece({7, 0}, {5, 0});
                movePiece({4, 0}, {6, 0});
              }

              return true;
            }
          }
        }
      } else if (sign(directionToMove.first) < 0 && color == Color::white) {
        // White queenside castle
        if (m_castleStatus[k_whiteQueensideIndex] != 0) {
          if (dynamic_cast<Rook *>(getPieceAt({0, 0})) &&
              !getPieceAt({0, 0})->hasMoved()) {
            if (!isPieceBlockingRook({0, 0}, {4, 0})) {
              if (!forMoveStorage) {
                m_castleStatus.reset();

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
    int sign = pieceToMove->getColor() == Color::white ? -1 : 1;
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
      if (end == m_enPassantSquare) {
        if ((pieceToMove->getPosition().first + 1) ==
                m_enPassantSquare->first ||
            (pieceToMove->getPosition().first - 1) ==
                m_enPassantSquare->first) {
          if (pieceToMove->getPosition().second - sign ==
              m_enPassantSquare->second) {
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
  int magnitude;
  bool movingVertically;
  Piece *potentialPiece;

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
    auto *potentialAttackerX = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerX);

    // Don't check own position
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<Rook *>(potentialAttackerX) ||
        dynamic_cast<Queen *>(potentialAttackerX)) {
      // Don't count own pieces
      CONTINUE_IF_VALID(potentialAttackerX->getColor() == color);

      if (!isPieceBlockingRook(potentialAttackerX->getPosition(), position)) {
        return true;
      }
    }
  }

  for (int i = 0; i < 8; ++i) {
    positionToCheck = {i, position.second};
    auto *potentialAttackerY = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerY);

    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<Rook *>(potentialAttackerY) ||
        dynamic_cast<Queen *>(potentialAttackerY)) {
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

    auto *potentialAttackerNortheast = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerNortheast);

    CONTINUE_IF_VALID(potentialAttackerNortheast->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<Bishop *>(potentialAttackerNortheast) ||
        dynamic_cast<Queen *>(potentialAttackerNortheast)) {
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

    auto *potentialAttackerNorthwest = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerNorthwest);

    CONTINUE_IF_VALID(potentialAttackerNorthwest->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<Bishop *>(potentialAttackerNorthwest) ||
        dynamic_cast<Queen *>(potentialAttackerNorthwest)) {
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

    auto *potentialAttackerSoutheast = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerSoutheast);

    CONTINUE_IF_VALID(potentialAttackerSoutheast->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<Bishop *>(potentialAttackerSoutheast) ||
        dynamic_cast<Queen *>(potentialAttackerSoutheast)) {
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

    auto *potentialAttackerSouthwest = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialAttackerSouthwest);

    CONTINUE_IF_VALID(potentialAttackerSouthwest->getColor() == color);
    CONTINUE_IF_VALID(position == positionToCheck);

    if (dynamic_cast<Bishop *>(potentialAttackerSouthwest) ||
        dynamic_cast<Queen *>(potentialAttackerSouthwest)) {
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

    auto *potentialKnightAttacker = getPieceAt(positionToCheck);

    CONTINUE_IF_NULL(potentialKnightAttacker);

    CONTINUE_IF_VALID(potentialKnightAttacker->getColor() == color);

    if (dynamic_cast<Knight *>(potentialKnightAttacker)) {
      return true;
    }
  }

  // Pawn case
  {
    int sign = (color == Color::white) ? 1 : -1;

    auto *potentialAttackerRight =
        getPieceAt({position.first + 1, position.second + sign});
    auto *potentialAttackerLeft =
        getPieceAt({position.first - 1, position.second + sign});

    if ((dynamic_cast<Pawn *>(potentialAttackerRight) &&
         (potentialAttackerRight->getColor() != color)) ||
        (dynamic_cast<Pawn *>(potentialAttackerLeft) &&
         (potentialAttackerLeft->getColor() != color))) {
      return true;
    }
  }

  // King case
  {
    for (int i = -1; i < 1; ++i) {
      auto *potentialAttackerHorizontal =
          getPieceAt({position.first + i, position.second});
      auto *potentialAttackerVertical =
          getPieceAt({position.first, position.second + i});
      auto *potentialAttackerUpwardDiagonal =
          getPieceAt({position.first + i, position.second + i});
      auto *potentialAttackerDownwardDiagonal =
          getPieceAt({position.first + i, position.second - i});

      if ((dynamic_cast<King *>(potentialAttackerHorizontal) &&
           (potentialAttackerHorizontal->getColor() != color)) ||
          (dynamic_cast<King *>(potentialAttackerVertical) &&
           (potentialAttackerVertical->getColor() != color)) ||
          (dynamic_cast<King *>(potentialAttackerUpwardDiagonal) &&
           (potentialAttackerUpwardDiagonal->getColor() != color)) ||
          (dynamic_cast<King *>(potentialAttackerDownwardDiagonal) &&
           (potentialAttackerDownwardDiagonal->getColor() != color))) {
        return true;
      }
    }
  }

  return false;
}

bool Board::isKingInCheck(Color color) {
  // Don't like this hardcoding but it's convenient
  const auto *king = (color == Color::white)
                         ? m_pieces[1][k_pawnsPerSide + 7].get()
                         : m_pieces[0][k_pawnsPerSide + 7].get();

  return isSquareAttacked(color, king->getPosition());
}

bool Board::canKingGetOutOfCheck(Color color, const Position &start,
                                 const Position &end) {
  if (dynamic_cast<King *>(getPieceAt(start))) {
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

  bool result;

  if (!pieceAtDestination) {
    pieceToMove->setPosition(end);
    result = isKingInCheck(color);
    pieceToMove->setPosition(start);
  } else {
    if (pieceToMove->getColor() == pieceAtDestination->getColor()) {
      return false;
    }
    // Exile to the shadow realm
    // 100% bad design
    pieceAtDestination->setPosition({-1, -1});
    pieceToMove->setPosition(end);
    result = isKingInCheck(color);
    pieceToMove->setPosition(start);
    pieceAtDestination->setPosition(end);
  }

  return result;
}

void Board::storeValidMoves() {
  if (k_verbose) {
    std::cout << "DEBUG: Legal moves for each piece:" << std::endl;
  }
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < k_totalPieces / 2; ++j) {
      CONTINUE_IF_NULL(m_pieces[i][j]);

      const auto &color = m_pieces[i][j]->getColor();
      const auto &position = m_pieces[i][j]->getPosition();

      if (k_verbose) {
        std::cout << std::endl;
        std::cout << m_pieces[i][j]->getLetter() << std::endl;
        std::cout << position.first << " " << position.second << std::endl;
      }

      // This isn't optimized at all. Every single board square is being
      // iterated through for each piece.
      // TODO: Update to check for piece type and then follow up with helper
      // functions like checkPawnMove()
      for (int k = 0; k < 8; ++k) {
        for (int l = 0; l < 8; ++l) {
          CONTINUE_IF_VALID(position == Position({k, l}));

          if (isValidMove(color, position, Position({k, l}), true)) {
            m_pieces[i][j]->addValidMove({k, l});
            m_allValidMoves.emplace_back(color, Position({k, l}));

            if (k_verbose) {
              std::cout << k << " " << l << std::endl;
            }
          }
        }
      }
    }
  }
}

bool Board::isKingCheckmated(Color color) {
  auto checkForColor = [this, color](std::pair<Color, Position> input) {
    return color == input.first;
  };

  if (isKingInCheck(color)) {
    if (std::find_if(m_allValidMoves.begin(), m_allValidMoves.end(),
                     checkForColor) == m_allValidMoves.end()) {
      return true;
    }
  }

  return false;
}

bool Board::hasStalemateOccurred(Color color) {
  // 50-move rule
  // if (m_halfMoveNum >= 50) {
  //   return true;
  // }

  // Dead positions
  // if (oneSideHas({PieceType::bishop, }))

  // Identical to checkmate function, but removes the requirement of the king
  // being in check
  auto checkForColor = [this, color](std::pair<Color, Position> input) {
    return color == input.first;
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

  if (piece == PieceType::knight) {
    m_pieces[index.first][index.second].reset(new Knight(position, color));
  } else if (piece == PieceType::bishop) {
    m_pieces[index.first][index.second].reset(new Bishop(position, color));
  } else if (piece == PieceType::rook) {
    m_pieces[index.first][index.second].reset(new Rook(position, color));
  } else if (piece == PieceType::queen) {
    m_pieces[index.first][index.second].reset(new Queen(position, color));
  }

  m_pawnToPromote.reset();

  return true;
}

void Board::updateBoardState(const Position &start, const Position &end) {
  // Clear board events that are only active for a single turn
  m_enPassantSquare.reset();
  m_pawnToPromote.reset();

  // Clear storage for moves to highlight
  m_movesToHighlight.clear();
  m_kingToHighlight.reset();
  m_pieceToHighlight.reset();

  // Refresh valid moves
  m_allValidMoves.clear();
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < k_totalPieces / 2; ++j) {
      CONTINUE_IF_NULL(m_pieces[i][j]);
      m_pieces[i][j]->clearValidMoves();
    }
  }
  storeValidMoves();

  auto *pieceThatMoved = getPieceAt(end);

  RETURN_IF_NULL(pieceThatMoved);

  if (dynamic_cast<Pawn *>(pieceThatMoved)) {
    int lastRow = pieceThatMoved->getColor() == Color::white ? 7 : 0;

    // Pawn moved two spaces - need to store en passant square
    if (std::abs(getDirectionVector(start, end).second) == 2) {
      int sign = (pieceThatMoved->getColor() == Color::white) ? -1 : 1;
      // Register en passant square as square directly behind pawn
      m_enPassantSquare = {end.first, end.second + sign};
    } else if (pieceThatMoved->getPosition().second == lastRow) {
      // Promotion case
      m_pawnToPromote = pieceThatMoved->getPosition();
    }
  }
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
  // Clear storage container first
  m_movesToHighlight.clear();
  m_pieceToHighlight.reset();

  auto *pieceToHighlight = getPieceAt(position);

  RETURN_IF_NULL(pieceToHighlight);

  m_pieceToHighlight = pieceToHighlight->getPosition();

  const auto &validMoves = pieceToHighlight->getValidMoves();

  if (validMoves.size() < 1) {
    // No possible moves
    return;
  }

  for (size_t i = 0; i < validMoves.size(); ++i) {
    m_movesToHighlight.emplace_back(pieceToHighlight->getColor(),
                                    validMoves.at(i));
  }
}

void Board::highlightKingInCheck(Color color) {
  // // Clear storage
  // m_kingToHighlight.reset();

  const auto *king = (color == Color::white)
                         ? m_pieces[1][k_pawnsPerSide + 7].get()
                         : m_pieces[0][k_pawnsPerSide + 7].get();

  m_kingToHighlight = king->getPosition();
}
