#ifndef DEFS_H
#define DEFS_H

#include <array>
#include <bitset>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

static bool k_verbose = false;

// SDL window default dimensions
constexpr int k_windowWidth = 800;
constexpr int k_windowHeight = 800;

// SDL board square dimensions
constexpr int k_squareWidth = k_windowWidth / 8;
constexpr int k_squareHeight = k_windowHeight / 8;

// Chess definitions
constexpr int k_totalPieces = 32;
constexpr int k_numCastleOptions = 4;

enum class Color { black, white };
enum class PieceType { none, pawn, knight, bishop, rook, queen, king };
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
  std::optional<Position> enPassantTarget;
  size_t halfMoveNum;
  size_t turnNum;
};

// Variables for indexing castle status
constexpr int k_blackKingsideIndex = 0;
constexpr int k_blackQueensideIndex = 1;
constexpr int k_whiteKingsideIndex = 2;
constexpr int k_whiteQueensideIndex = 3;

#endif // DEFS_H
