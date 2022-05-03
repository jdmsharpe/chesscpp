#ifndef BOARD_H
#define BOARD_H

#include "Pieces.h"

class Board
{
public:
    Board();

    ~Board() {}

    void display();

    Piece *getPieceAt(const Position &position);

    void capturePiece(const Position &position);

    bool isValidMove(Color color, const Position &start, const Position &end);

    void movePiece(const Position &start, const Position &end);

    bool isPieceAttacked(Color color, const Position& position);

    bool isKingInCheck(Color color);

    bool willKingBeInCheck(Color color, const Position& start, const Position &end);

private:
    inline Position getDirectionVector(const Position &start, const Position &end)
    {
        return {end.first - start.first, end.second - start.second};
    }

    bool isPieceBlockingBishop(const Position &start, const Position &end);

    bool isPieceBlockingRook(const Position &start, const Position &end);

    using Pieces = std::array<std::array<std::unique_ptr<Piece>, k_totalPieces / 2>, 2>;
    Pieces m_pieces;
};

#endif // BOARD_H
