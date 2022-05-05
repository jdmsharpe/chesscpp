#ifndef DEFS_H
#define DEFS_H

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

constexpr int k_totalPieces = 32;

enum class Color { black, white };

typedef std::pair<int, int> Position;

using PieceContainer = std::vector<std::pair<Color, Position>>;
struct BoardLayout {
    PieceContainer pawns;
    PieceContainer knights;
    PieceContainer bishops;
    PieceContainer rooks;
    PieceContainer queens;
    PieceContainer kings;

    Color turn;
    Position enPassantTarget;
    size_t halfMoveNum;
    size_t turnNum;

};

#endif // DEFS_H
