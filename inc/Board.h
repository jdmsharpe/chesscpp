#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Macros.h"
#include "Pieces.h"

// Class to define the board state and manage updates to it
// Responsible for checking move validity, handling special behavior like
// check, promotion, etc., and rendering the board and pieces on the window
// TODO: This class became way too big. Needs to be refactored into 3 or more
// classes
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

  void sdlDisplay(Color color);

  Piece *getPieceAt(const Position &position);

  bool isValidMove(Color color, const Position &start, const Position &end,
                   const bool forMoveStorage);

  bool isKingInCheck(Color color);

  bool canKingGetOutOfCheck(Color color, const Position &start,
                            const Position &end);

  bool isKingCheckmated(Color color);

  bool hasStalemateOccurred(Color color);

  inline bool pawnToPromote() const { return m_pawnToPromote.has_value(); }

  // For legacy game
  bool promotePawn(const PieceType &piece);

  // For SDL game
  bool promotePawn(const Position &positionToPromoteTo);

  void sdlDisplayPromotionOptions(Color color) const;

  void movePiece(const Position &start, const Position &end);

  void updateBoardState(const Position &start, const Position &end);

  bool isInputValid(Color color, const std::queue<Position> &positions);

  void highlightPotentialMoves(const Position &position);

  void highlightKingInCheck(Color color);

  inline void clearOldPieceHighlight() {
    m_movesToHighlight.clear();
    m_pieceToHighlight.reset();
  }

  inline void clearOldKingHighlight() { m_kingToHighlight.reset(); }

  inline const std::vector<FullMove> &getAllValidMoves() const {
    return m_allValidMoves;
  }

  const std::vector<FullMove> getValidMovesFor(Color color) const;

  void refreshValidMoves();

  const LumpedBoardAndGameState &
  getBoardAndGameState(Color color, size_t halfMoveNum = 0, size_t turnNum = 1);

  void testMove(const Position &start, const Position &end, int depth = 10);

  void undoMove(const Position &start, const Position &end, int depth = 10);

  inline CastleStatus getCastleStatus() const { return m_castleStatus; }

  inline void setComputerPlaying(const bool isPlaying) {
    m_isComputerPlaying = isPlaying;
  }

  inline size_t getFiftyMoveRuleCount() const { return m_fiftyMoveRuleCount; }

private:
  void sdlDrawSquare(const Position &position, const SDL_Color &sdlColor) const;

  void sdlDrawPiece(Color color, const Piece *piece, int verticalOffset) const;

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

  // Black's last move to highlight
  std::optional<Position> m_blackLastMoveHighlight = std::nullopt;

  // White's last move to highlight
  std::optional<Position> m_whiteLastMoveHighlight = std::nullopt;

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
