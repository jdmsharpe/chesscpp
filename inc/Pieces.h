#ifndef PIECES_H
#define PIECES_H

#include "Defs.h"

class Piece {
public:
  Piece(Position position, Color color, char &&letter)
      : m_position(position), m_startingPosition(position), m_color(color),
        m_letter(letter), m_hasMoved(false) {}
  virtual ~Piece() = default;

  virtual bool isValidMove(const Position &move) const;

  inline Position getPosition() const { return m_position; }

  inline void setPosition(const Position &position) {
    if (!m_hasMoved) {
      m_hasMoved = true;
    }

    m_position = position;
  }

  inline Color getColor() const { return m_color; }

  inline void setColor(const Color &color) { m_color = color; }

  inline char getLetter() const { return m_letter; }

  inline bool hasMoved() const { return m_hasMoved; }

protected:
  Position m_position;
  Position m_startingPosition;
  Color m_color;
  const char m_letter;
  bool m_hasMoved;
};

class Pawn : public Piece {
public:
  Pawn(Position position, Color color)
      : Piece(position, color, color == Color::black ? 'p' : 'P') {}
  ~Pawn() {}

  bool isValidMove(const Position &move) const override;
};

class Knight : public Piece {
public:
  Knight(Position position, Color color)
      : Piece(position, color, color == Color::black ? 'n' : 'N') {}
  ~Knight() {}

  bool isValidMove(const Position &move) const override;
};

class Bishop : public Piece {
public:
  Bishop(Position position, Color color)
      : Piece(position, color, color == Color::black ? 'b' : 'B') {}
  ~Bishop() {}

  bool isValidMove(const Position &move) const override;
};

class Rook : public Piece {
public:
  Rook(Position position, Color color)
      : Piece(position, color, color == Color::black ? 'r' : 'R') {}
  ~Rook() {}

  bool isValidMove(const Position &move) const override;
};

class Queen : public Piece {
public:
  Queen(Position position, Color color)
      : Piece(position, color, color == Color::black ? 'q' : 'Q') {}
  ~Queen() {}

  bool isValidMove(const Position &move) const override;
};

class King : public Piece {
public:
  King(Position position, Color color)
      : Piece(position, color, color == Color::black ? 'k' : 'K') {}
  ~King() {}

  bool isValidMove(const Position &move) const override;
};

#endif // PIECES_H
