#include "Pieces.h"

bool Piece::isValidMove(const Position &move) const
{
    if (m_position == move)
    {
        return false;
    }

    if (move.first >= 8 || move.second >= 8 || move.first < 0 || move.second < 0)
    {
        return false;
    }

    return true;
}

bool Pawn::isValidMove(const Position &move) const
{
    int sign = (m_color == Color::white) ? -1 : 1;

    if (m_startingPosition == m_position)
    {
        if (m_position.second == move.second + (sign * 2))
        {
            return true;
        }
    }

    if (m_position.second != move.second + sign)
    {
        return false;
    }

    return Piece::isValidMove(move);
}

bool Knight::isValidMove(const Position &move) const
{
    if (std::abs((m_position.first - move.first) * (m_position.second - move.second)) != 2)
    {
        return false;
    }

    return Piece::isValidMove(move);
}

bool Bishop::isValidMove(const Position &move) const
{
    if ((move.first - m_position.first != move.second - m_position.second) &&
        (move.first - m_position.first != -move.second + m_position.second))
    {
        return false;
    }

    return Piece::isValidMove(move);
}

bool Rook::isValidMove(const Position &move) const
{
    if ((m_position.first != move.first) && (m_position.second != move.second))
    {
        return false;
    }

    return Piece::isValidMove(move);
}

bool Queen::isValidMove(const Position &move) const
{
    // Combine cases from bishop and rook
    if (((move.first - m_position.first != move.second - m_position.second) &&
         (move.first - m_position.first != -move.second + m_position.second)) &&
        ((m_position.first != move.first) && (m_position.second != move.second)))
    {
        return false;
    }

    return Piece::isValidMove(move);
}

bool King::isValidMove(const Position &move) const
{
    if (std::abs(m_position.first - move.first) > 1 || std::abs(m_position.second - move.second) > 1)
    {
        return false;
    }

    return Piece::isValidMove(move);
}
