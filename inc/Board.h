#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Macros.h"
#include "Pieces.h"

class Board {
public:
  Board() {}

  ~Board() {}

  inline void setRenderer(SDL_Renderer *renderer) {
    RETURN_IF_NULL(renderer);
    m_renderer = renderer;
  }

  void loadGame();

  void loadFromFen(const BoardLayout &layout);

  void cliDisplay(Color color);

  void sdlDisplay();

  bool isValidMove(Color color, const Position &start, const Position &end,
                   const bool forMoveStorage);

  bool isKingInCheck(Color color);

  bool canKingGetOutOfCheck(Color color, const Position &start,
                            const Position &end);

  bool isKingCheckmated(Color color);

  bool hasStalemateOccurred(Color color);

  inline bool pawnToPromote() const { return m_pawnToPromote.has_value(); }

  bool promotePawn(const PieceType &piece);

  void movePiece(const Position &start, const Position &end);

  void updateBoardState(const Position &start, const Position &end);

private:
  Piece *getPieceAt(const Position &position);

  std::pair<size_t, size_t> getIndexOfPiece(const Piece *piece);

  void capturePiece(const Position &position);

  void storeValidMoves();

  bool isPieceBlockingBishop(const Position &start, const Position &end);

  bool isPieceBlockingRook(const Position &start, const Position &end);

  bool isSquareAttacked(Color color, const Position &position);

  bool moveAndCheckForCheck(Color color, const Position &start,
                            const Position &end);

  void setKingCastleStatus(Color color, CastleSide side);

  SDL_Renderer *m_renderer = nullptr;

  using Pieces =
      std::array<std::array<std::unique_ptr<Piece>, k_totalPieces / 2>, 2>;
  Pieces m_pieces;

  using Moves = PieceContainer;
  Moves m_allValidMoves = {};

  CastleStatus m_castleStatus = CastleStatus().set();
  std::optional<Position> m_enPassantSquare = std::nullopt;
  std::optional<Position> m_pawnToPromote = std::nullopt;

  size_t m_halfMoveNum = 0;
  size_t m_turnNum = 1;
};

#endif // BOARD_H
