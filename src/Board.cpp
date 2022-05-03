#include "Board.h"
#include "Macros.h"

namespace
{

    constexpr int k_pawnsPerSide = k_totalPieces / 4;

    constexpr std::array<Position, 8> k_potentialKnightPositions = {{{2, 1}, {2, -1}, {-2, -1}, {-2, 1}, {1, 2}, {1, -2}, {-1, -2}, {-1, 2}}};

    template <class T>
    std::unique_ptr<T> makePiece(int xPos, int yPos, Color color)
    {
        return std::move(std::make_unique<T>(std::make_pair(xPos, yPos), color));
    }

    // Credit where credit is due: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
    template <typename T>
    int sign(T val)
    {
        return (T(0) < val) - (val < T(0));
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

void Board::display()
{
    for (int i = 7; i >= 0; --i)
    {
        for (int j = 0; j < 8; ++j)
        {
            const auto *piece = getPieceAt({j, i});
            if (!piece)
            {
                std::cout << ".";
            }
            else
            {
                std::cout << piece->getLetter();
            }

            if (j == 7)
            {
                std::cout << std::endl;
            }
        }
    }
}

Piece *Board::getPieceAt(const Position &position)
{
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < k_totalPieces / 2; ++j)
            {
                CONTINUE_IF_NULL(m_pieces[i][j]);
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

void Board::capturePiece(const Position &position)
{
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < k_totalPieces / 2; ++j)
            {
                CONTINUE_IF_NULL(m_pieces[i][j]);
                if (m_pieces[i][j]->getPosition() == position)
                {
                    m_pieces[i][j].reset();
                    m_pieces[i][j] = nullptr;
                }
            }
        }
    }
}

bool Board::isValidMove(Color color, const Position &start, const Position &end)
{
    auto *pieceToMove = getPieceAt(start);
    auto *pieceAtDestination = getPieceAt(end);

    // Nothing there
    if (!pieceToMove)
    {
        return false;
    }

    // Can't move pieces of the opposing color
    if (pieceToMove->getColor() != color)
    {
        return false;
    }

    // First see if the king is in check
    // Then see if king will be in check
    if (isKingInCheck(color))
    {
        if (willKingBeInCheck(color, start, end))
        {
            return false;
        }
    }


    // Castling case

    // Pawns are allowed to move diagonally only if there's a piece to capture
    // En passant case should go here too
    if (dynamic_cast<Pawn *>(pieceToMove))
    {
        if (pieceAtDestination)
        {
            if (pieceToMove->getColor() != pieceAtDestination->getColor())
            {
                int sign = pieceToMove->getColor() == Color::white ? -1 : 1;
                if ((start.second == end.second + sign) &&
                    ((start.first == end.first + 1) ||
                     (start.first == end.first - 1)))
                {
                    return true;
                }
            }
        }
    }

    // Most obvious case - check if requested move is in agreement with piece logic
    if (!pieceToMove->isValidMove(end))
    {
        return false;
    }

    // Check for pieces blocking path
    if (dynamic_cast<Bishop *>(pieceToMove))
    {
        if (isPieceBlockingBishop(start, end))
        {
            return false;
        }
    }

    if (dynamic_cast<Rook *>(pieceToMove))
    {
        if (isPieceBlockingRook(start, end))
        {
            return false;
        }
    }

    if (dynamic_cast<Queen *>(pieceToMove))
    {
        Position directionVector = getDirectionVector(start, end);
        if (std::abs(directionVector.first) == std::abs(directionVector.second))
        {
            if (isPieceBlockingBishop(start, end))
            {
                return false;
            }
        }
        else
        {
            if (isPieceBlockingRook(start, end))
            {
                return false;
            }
        }
    }

    // General capture case
    if (pieceAtDestination)
    {
        if (pieceToMove->getColor() != pieceAtDestination->getColor())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}

void Board::movePiece(const Position &start, const Position &end)
{
    auto *pieceToMove = getPieceAt(start);
    auto *pieceAtDestination = getPieceAt(end);

    // All cases should be checked at this point
    // Capturing correctly is assumed
    if (pieceAtDestination)
    {
        capturePiece(end);
        pieceToMove->setPosition(end);
    }
    else
    {
        pieceToMove->setPosition(end);
    }
}

bool Board::isPieceBlockingBishop(const Position &start, const Position &end)
{
    Position directionVector = getDirectionVector(start, end);
    // Direction vector components will be equal other than sign
    int magnitude = std::abs(directionVector.first);
    Piece *potentialPiece;

    for (int i = 1; i <= magnitude; ++i)
    {
        potentialPiece = getPieceAt({start.first + (i * sign(directionVector.first)), start.second + (i * sign(directionVector.second))});

        // Piece found
        if (potentialPiece && potentialPiece->getPosition() != end)
        {
            return true;
        }
    }

    return false;
}

bool Board::isPieceBlockingRook(const Position &start, const Position &end)
{
    Position directionVector = getDirectionVector(start, end);
    // One component of vector will be zero
    int magnitude;
    bool movingVertically;
    Piece *potentialPiece;

    if (directionVector.first == 0)
    {
        magnitude = std::abs(directionVector.second);
        movingVertically = true;
    }
    else if (directionVector.second == 0)
    {
        magnitude = std::abs(directionVector.first);
        movingVertically = false;
    }

    for (int i = 1; i <= magnitude; ++i)
    {
        if (movingVertically)
        {
            potentialPiece = getPieceAt({start.first, start.second + (i * sign(directionVector.second))});
        }
        else
        {
            potentialPiece = getPieceAt({start.first + (i * sign(directionVector.first)), start.second});
        }

        // Piece found
        if (potentialPiece && potentialPiece->getPosition() != end)
        {
            return true;
        }
    }

    return false;
}

bool Board::isPieceAttacked(Color color, const Position& position) {
    // Rook case
    // Should be a way to refactor/combine these...
    for (int i = 0; i < 8; ++i)
    {
        Position positionToCheck = {position.first, i};
        auto *potentialAttackerX = getPieceAt(positionToCheck);

        CONTINUE_IF_NULL(potentialAttackerX);

        // Don't check own position
        CONTINUE_IF_TRUE(position == positionToCheck);

        if (dynamic_cast<Rook *>(potentialAttackerX) || dynamic_cast<Queen *>(potentialAttackerX))
        {
            // Don't count own pieces
            CONTINUE_IF_TRUE(potentialAttackerX->getColor() == color);

            if (!isPieceBlockingRook(potentialAttackerX->getPosition(), position))
            {
                return true;
            }
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        Position positionToCheck = {i, position.second};
        auto *potentialAttackerY = getPieceAt(positionToCheck);

        CONTINUE_IF_NULL(potentialAttackerY);

        CONTINUE_IF_TRUE(position == positionToCheck);

        if (dynamic_cast<Rook *>(potentialAttackerY) || dynamic_cast<Queen *>(potentialAttackerY))
        {
            CONTINUE_IF_TRUE(potentialAttackerY->getColor() == color);   

            if (!isPieceBlockingRook(potentialAttackerY->getPosition(), position))
            {
                return true;
            }
        }
    }

    // Bishop case
    // The case for refactoring the rook case doubly applies here
    for (int i = 1; i < 8; ++i)
    {
        if (position.first + i > 7 || position.second + i > 7) {
            continue;
        }

        auto *potentialAttackerNortheast = getPieceAt({position.first + i, position.second + i});

        CONTINUE_IF_NULL(potentialAttackerNortheast);

        CONTINUE_IF_TRUE(potentialAttackerNortheast->getColor() == color);

        if (dynamic_cast<Bishop *>(potentialAttackerNortheast) ||
            dynamic_cast<Queen *>(potentialAttackerNortheast))
        {
            if (!isPieceBlockingBishop(potentialAttackerNortheast->getPosition(), position))
            {
                {
                    return true;
                }
            }
        }
    }

    for (int i = 1; i < 8; ++i)
    {
        if (position.first - i < 0 || position.second + i > 7)
        {
            continue;
        }

        auto *potentialAttackerNorthwest = getPieceAt({position.first - i, position.second + i});

        CONTINUE_IF_NULL(potentialAttackerNorthwest);

        CONTINUE_IF_TRUE(potentialAttackerNorthwest->getColor() == color);

        if (dynamic_cast<Bishop *>(potentialAttackerNorthwest) ||
            dynamic_cast<Queen *>(potentialAttackerNorthwest))
        {
            if (!isPieceBlockingBishop(potentialAttackerNorthwest->getPosition(), position))
            {
                {
                    return true;
                }
            }
        }
    }

    for (int i = 1; i < 8; ++i)
    {
        if (position.first + i > 7 || position.second - i < 0)
        {
            continue;
        }

        auto *potentialAttackerSoutheast = getPieceAt({position.first + i, position.second - i});

        CONTINUE_IF_NULL(potentialAttackerSoutheast);

        CONTINUE_IF_TRUE(potentialAttackerSoutheast->getColor() == color);

        if (dynamic_cast<Bishop *>(potentialAttackerSoutheast) ||
            dynamic_cast<Queen *>(potentialAttackerSoutheast))
        {
            if (!isPieceBlockingBishop(potentialAttackerSoutheast->getPosition(), position))
            {
                {
                    return true;
                }
            }
        }
    }

    for (int i = 1; i < 8; ++i)
    {
        if (position.first - i < 0 || position.second - i < 0)
        {
            continue;
        }

        auto *potentialAttackerSouthwest = getPieceAt({position.first - i, position.second - i});

        CONTINUE_IF_NULL(potentialAttackerSouthwest);

        CONTINUE_IF_TRUE(potentialAttackerSouthwest->getColor() == color);

        if (dynamic_cast<Bishop *>(potentialAttackerSouthwest) ||
            dynamic_cast<Queen *>(potentialAttackerSouthwest))
        {
            if (!isPieceBlockingBishop(potentialAttackerSouthwest->getPosition(), position))
            {
                {
                    return true;
                }
            }
        }
    }

    // Knight case
    for (int i = 0; i < k_potentialKnightPositions.size(); ++i)
    {
        auto *potentialKnightAttacker = getPieceAt({position.first + k_potentialKnightPositions[i].first, position.second + k_potentialKnightPositions[i].second});

        CONTINUE_IF_NULL(potentialKnightAttacker);

        CONTINUE_IF_TRUE(potentialKnightAttacker->getColor() == color);

        if (dynamic_cast<Knight *>(potentialKnightAttacker))
        {
            return true;
        }
    }

    // Pawn case
    {
        int sign = (color == Color::white) ? 1 : -1;

        auto *potentialAttackerRight = getPieceAt({position.first + 1, position.second + sign});
        auto *potentialAttackerLeft = getPieceAt({position.first - 1, position.second + sign});

        if ((dynamic_cast<Pawn *>(potentialAttackerRight) && (potentialAttackerRight->getColor() != color)) ||
            (dynamic_cast<Pawn *>(potentialAttackerLeft) && (potentialAttackerLeft->getColor() != color)))
        {
            return true;
        }
    }

    return false;
}

bool Board::isKingInCheck(Color color)
{
    // Don't like this hardcoding but it's convenient
    const auto *king = (color == Color::white) ? m_pieces[1][k_pawnsPerSide + 7].get() : m_pieces[0][k_pawnsPerSide + 7].get();

    return isPieceAttacked(color, king->getPosition());
}

bool Board::willKingBeInCheck(Color color, const Position &start, const Position &end)
{
    if (dynamic_cast<King *>(getPieceAt(start)))
    {
        // See if this move gets king out of check
        return isPieceAttacked(color, end);
    }

    // TODO: This misses the case to check if the move blocks check
    // Think of a good way to add this in without overwriting board state
    return isKingInCheck(color);
}
