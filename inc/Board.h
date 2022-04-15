#ifndef BOARD_H
#define BOARD_H

#include "Pieces.h"

class Board
{
public:
    Board();

    ~Board() {}

    Piece *getPieceAt(const Position &position);

    bool HandleMove(const Position& start, const Position& end);

private:
    using Pieces = std::array<std::array<std::unique_ptr<Piece>, k_totalPieces / 2>, 2>;
    Pieces m_pieces;
};

#endif // BOARD_H
