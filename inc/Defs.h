#ifndef DEFS_H
#define DEFS_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

// Enables debug logging
static bool k_verbose = false;

// Increments after every application run() call
static long k_counter = 0;

// SDL window default dimensions
constexpr int k_windowWidth = 800;
constexpr int k_windowHeight = 800;

// SDL board square dimensions
constexpr int k_squareWidth = k_windowWidth / 8;
constexpr int k_squareHeight = k_windowHeight / 8;

// Chess definitions
constexpr int k_totalPieces = 32;
constexpr int k_totalSquares = 64;
constexpr int k_numCastleOptions = 4;

// For preallocating vectors
constexpr int k_maxPawnsPossible = 8;
constexpr int k_maxKnightsPossible = 10;
constexpr int k_maxBishopsPossible = 10;
constexpr int k_maxRooksPossible = 10;
constexpr int k_maxQueensPossible = 9;
constexpr int k_maxKingsPossible = 1;

// For indexing castle status
constexpr int k_blackKingsideIndex = 0;
constexpr int k_blackQueensideIndex = 1;
constexpr int k_whiteKingsideIndex = 2;
constexpr int k_whiteQueensideIndex = 3;

enum class Color { black, white };
enum class PieceType { none, pawn, knight, bishop, rook, queen, king };
enum class CastleSide { kingside, queenside };

// Zero-indexed representation of a square's position
typedef std::pair<int, int> Position;

using PieceContainer = std::vector<std::pair<Color, Position>>;
using CastleStatus = std::bitset<k_numCastleOptions>;
using EnPassantStatus = std::optional<std::pair<Color, Position>>;

// Representation of the entirety of both board and game states.
struct LumpedBoardAndGameState {
  // Board state
  PieceContainer pawns;
  PieceContainer knights;
  PieceContainer bishops;
  PieceContainer rooks;
  PieceContainer queens;
  PieceContainer kings;
  // In order: black kingside,
  //           black queenside,
  //           white kingside,
  //           white queenside
  CastleStatus castleStatus;
  EnPassantStatus enPassantStatus;

  // Game state
  Color whoseTurn;
  size_t halfMoveNum;
  size_t turnNum;

  // Default ctor assumes start of game
  LumpedBoardAndGameState()
      : whoseTurn(Color::white), halfMoveNum(0), turnNum(1) {
    pawns.reserve(k_maxPawnsPossible);
    knights.reserve(k_maxKnightsPossible);
    bishops.reserve(k_maxBishopsPossible);
    rooks.reserve(k_maxRooksPossible);
    queens.reserve(k_maxQueensPossible);
    kings.reserve(k_maxKingsPossible);

    castleStatus.set();
    enPassantStatus.reset();
  }
};

// A full move in this case contains all possible information about a
// potential move: the color, piece type, and starting and ending positions
// This is particularly useful for the computer player
struct FullMove {
  PieceType pieceType;
  Color color;
  Position start;
  Position end;

  FullMove(PieceType pieceType, Color color, Position start, Position end)
      : pieceType(pieceType), color(color), start(start), end(end) {}
  FullMove() {}
};

// For some reason this has to be static?
Color static getOtherColor(Color color) {
  return (color == Color::white) ? Color::black : Color::white;
}

#endif // DEFS_H
