#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Macros.h"
#include "Pieces.h"

// Class to define the board state and manage updates to it
// Responsible for checking move validity, handling special behavior like
// check, promotion, etc., and rendering the board and pieces on the window
// TODO: Refactor into two classes - maybe one responsible for SDL stuff
class Board {
public:
  Board() = default;

  ~Board();

  inline void setRenderer(SDL_Renderer *renderer) {
    RETURN_IF_NULL(renderer);
    m_renderer = renderer;
  }

  void loadTextures();

  void loadGame();

  void loadFromState(const LumpedBoardAndGameState &state);

  void cliDisplay(Color color);

  void sdlDisplay();

  Piece *getPieceAt(const Position &position);

  bool isValidMove(Color color, const Position &start, const Position &end,
                   const bool forMoveStorage);

  bool isKingInCheck(Color color);

  bool canKingGetOutOfCheck(Color color, const Position &start,
                            const Position &end);

  bool isKingCheckmated(Color color);

  bool hasStalemateOccurred(Color color);

  inline bool pawnToPromote() const { return m_pawnToPromote.has_value(); }

  bool promotePawn(const PieceType &piece);

  void movePiece(const Position &start, const Position &end);

  void updateBoardState(const Position &start, const Position &end);

  bool isInputValid(Color color, const std::queue<Position> &positions);

  void highlightPotentialMoves(const Position &position);

  void highlightKingInCheck(Color color);

  inline void clearOldPieceHighlight() {
    m_movesToHighlight.clear();
    m_pieceToHighlight.reset();
  }

  inline void clearOldKingHighlight() {
    m_kingToHighlight.reset();
  }

  inline const std::vector<FullMove> &getAllValidMoves() const {
    return m_allValidMoves;
  }

  const std::vector<FullMove> getValidMovesFor(Color color) const;

  void refreshValidMoves();

  const LumpedBoardAndGameState &
  getBoardAndGameState(Color color, size_t halfMoveNum = 0, size_t turnNum = 1);

  void testMove(const Position &start, const Position &end, int depth = 10);

  void undoMove(const Position &start, const Position &end, int depth = 10);

  inline CastleStatus getCastleStatus() const {
    return m_castleStatus;
  }

  inline void setComputerPlaying(const bool isPlaying) {
    m_isComputerPlaying = isPlaying;
  }

private:
  void sdlDrawSquare(const Position &position, const SDL_Color &sdlColor);

  void sdlDrawPiece(const Piece *piece);

  std::pair<size_t, size_t> getIndexOfPiece(const Piece *piece);

  void capturePiece(const Position &position);

  void storeValidMoves();

  bool isPieceBlockingBishop(const Position &start, const Position &end);

  bool isPieceBlockingRook(const Position &start, const Position &end);

  bool isSquareAttacked(Color color, const Position &position);

  bool moveAndCheckForCheck(Color color, const Position &start,
                            const Position &end);

  void setKingCastleStatus(Color color, CastleSide side);

  const Piece *getKingFromColor(Color color) const;

  bool checkForDeadPosition() const;

  SDL_Renderer *m_renderer = NULL;

  // Holds texture of image with all piece sprites
  SDL_Texture *m_pieceImageTexture = NULL;

  // First dimension of 2D vector is for each color
  using Pieces = std::vector<std::vector<std::unique_ptr<Piece>>>;
  // Container for every piece
  Pieces m_pieces = {};

  LumpedBoardAndGameState m_boardAndGameState = {};

  // Container for all possible moves on the board
  std::vector<FullMove> m_allValidMoves = {};

  // Squares to highlight as valid moves
  std::vector<Position> m_movesToHighlight = {};

  // Piece square to highlight if clicked
  std::optional<Position> m_pieceToHighlight = std::nullopt;

  // King square to highlight if in check
  std::optional<Position> m_kingToHighlight = std::nullopt;

  // Both sides' castling availability
  CastleStatus m_castleStatus = CastleStatus().set();

  // Stored pawn color and en passant square, if any
  EnPassantStatus m_enPassantStatus = std::nullopt;

  // Stored pawn to promote, if any
  std::optional<Position> m_pawnToPromote = std::nullopt;

  bool m_isComputerPlaying = false;

  bool m_pawnMovedOrPieceCaptured = false;

  size_t m_fiftyMoveRuleCount = 0;
};

#endif // BOARD_H
