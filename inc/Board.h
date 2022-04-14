#ifndef BOARD_H
#define BOARD_H

#include "Pieces.h"

class Board
{
public:
    Board();

    ~Board() {}

    Piece *getPieceAt(const Position &position)
    {
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < k_totalPieces / 2; ++j) {
                if (m_pieces[i][j]->getPosition() == position) {
                    return m_pieces[i][j].get();
                }
            }
        }

        return nullptr;
    }

private:
    using Pieces = std::array<std::array<std::unique_ptr<Piece>, k_totalPieces / 2>, 2>;
    Pieces m_pieces;
};

#endif // BOARD_H