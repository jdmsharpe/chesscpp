#ifndef BOARD_H
#define BOARD_H

#include "Pieces.h"

class Board {
public:
  Board() {}

  virtual ~Board() = default;

  void loadGame();

  void loadFromFen(const BoardLayout &layout);

  void display();

  bool isValidMove(Color color, const Position &start, const Position &end);

  bool isKingInCheck(Color color);

  bool willKingBeInCheck(Color color, const Position &start,
                         const Position &end);

  // bool isKingCheckmated(Color color);

  inline bool pawnToPromote() const { return m_pawnToPromote.has_value(); }

  bool promotePawn(const PieceType &piece);

  void movePiece(const Position &start, const Position &end);

  void updateAfterMove(const Position &start, const Position &end);

private:
  inline Position getDirectionVector(const Position &start,
                                     const Position &end) {
    return {end.first - start.first, end.second - start.second};
  }

  Piece *getPieceAt(const Position &position);

  std::pair<size_t, size_t> getIndexOfPiece(const Piece *piece);

  void capturePiece(const Position &position);

  bool isPieceBlockingBishop(const Position &start, const Position &end);

  bool isPieceBlockingRook(const Position &start, const Position &end);

  bool isSquareAttacked(Color color, const Position &position);

  void setKingCastleStatus(Color color, CastleSide side);

  using Pieces =
      std::array<std::array<std::unique_ptr<Piece>, k_totalPieces / 2>, 2>;
  Pieces m_pieces;

  CastleStatus m_castleStatus;
  std::optional<Position> m_enPassantSquare;
  std::optional<Position> m_pawnToPromote;
};

#endif // BOARD_H
