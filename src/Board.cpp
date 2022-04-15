#include "Board.h"

namespace {

    constexpr int k_pawnsPerSide = k_totalPieces / 4;

    template <class T>
    std::unique_ptr<T> makePiece(int xPos, int yPos, Color color)
    {
        return std::move(std::make_unique<T>(std::make_pair(xPos, yPos), color));
    }

} // namespace

Board::Board()
{
    for (int i = 0; i < k_pawnsPerSide; ++i)
    {
        m_pieces[1][i] = makePiece<Pawn>(i, 1, Color::white);
    }

    m_pieces[1][k_pawnsPerSide] = makePiece<Rook>(0, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 1] = makePiece<Rook>(7, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 2] = makePiece<Knight>(1, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 3] = makePiece<Knight>(6, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 4] = makePiece<Bishop>(2, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 5] = makePiece<Bishop>(5, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 6] = makePiece<Queen>(3, 0, Color::white);
    m_pieces[1][k_pawnsPerSide + 7] = makePiece<King>(4, 0, Color::white);

    for (int i = 0; i < k_pawnsPerSide; ++i)
    {
        m_pieces[0][i] = makePiece<Pawn>(i, 6, Color::black);
    }

    m_pieces[0][k_pawnsPerSide] = makePiece<Rook>(0, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 1] = makePiece<Rook>(7, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 2] = makePiece<Knight>(1, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 3] = makePiece<Knight>(6, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 4] = makePiece<Bishop>(2, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 5] = makePiece<Bishop>(5, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 6] = makePiece<Queen>(3, 7, Color::black);
    m_pieces[0][k_pawnsPerSide + 7] = makePiece<King>(4, 7, Color::black);
}

Piece *Board::getPieceAt(const Position &position)
{
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < k_totalPieces / 2; ++j)
            {
                // Doesn't really feel optimal, but it does work
                if (m_pieces[i][j]->getPosition() == position)
                {
                    return m_pieces[i][j].get();
                }
            }
        }

        return nullptr;
    }
}

bool Board::HandleMove(const Position &start, const Position &end)
{
    auto *pieceToMove = getPieceAt(start);

    // Most obvious case - check if requested move is in agreement with piece logic
    if (!pieceToMove->isValidMove(end))
    {
        return false;
    }
    else
    {
        // // Pawns are allowed to move diagonally only if there's a piece to capture
        // if (pieceToMove->getLetter() == 'p' && getPieceAt(end))
        // {
        //     int sign = pieceToMove->getColor() == Color::white ? -1 : 1;
        //     if ((start.second == end.second + sign) && )
        // }
    }
}
