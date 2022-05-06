#ifndef DEFS_H
#define DEFS_H

#include <array>
#include <bitset>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

constexpr int k_totalPieces = 32;
constexpr int k_numCastleOptions = 4;

enum class Color { black, white };
enum class CastleSide { kingside, queenside };

typedef std::pair<int, int> Position;

using PieceContainer = std::vector<std::pair<Color, Position>>;
using CastleStatus = std::bitset<k_numCastleOptions>;
struct BoardLayout {
  PieceContainer pawns;
  PieceContainer knights;
  PieceContainer bishops;
  PieceContainer rooks;
  PieceContainer queens;
  PieceContainer kings;

  Color whoseTurn;
  // In order: black kingside,
  //           black queenside,
  //           white kingside,
  //           white queenside
  CastleStatus castleStatus;
  Position enPassantTarget;
  size_t halfMoveNum;
  size_t turnNum;
};

#endif // DEFS_H
