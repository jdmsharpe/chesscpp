#include "Board.h"
#include "Game.h"

#include <gtest/gtest.h>

namespace {

const std::string k_testFenFilepath = "../../chess/test/test.fen";

const std::bitset<k_numCastleOptions> k_bothSidesCastleRights =
    std::bitset<k_numCastleOptions>().set();
const std::bitset<k_numCastleOptions> k_blackLostQueensideCastleRights =
    std::bitset<k_numCastleOptions>(std::string("1101"));
const std::bitset<k_numCastleOptions> k_blackOnlyKingsideCastleRights =
    std::bitset<k_numCastleOptions>(std::string("0001"));
const std::bitset<k_numCastleOptions> k_blackOnlyCastleRights =
    std::bitset<k_numCastleOptions>(std::string("0011"));
const std::bitset<k_numCastleOptions> k_whiteOnlyCastleRights =
    std::bitset<k_numCastleOptions>(std::string("1100"));
const std::bitset<k_numCastleOptions> k_neitherSideCastleRights =
    std::bitset<k_numCastleOptions>().reset();

} // namespace

class TestBoard : public ::testing::Test {
public:
  TestBoard() {}

  static void SetUpTestSuite() {
    if (m_board == nullptr) {
      m_board = new Board();
      m_board->loadGame();
    }

    if (m_game == nullptr) {
      m_game = new Game();
    }
  }

  static void TearDownTestSuite() {
    delete m_board;
    m_board = nullptr;
  }

protected:
  void SetUp() override {}
  void TearDown() override {}

  static Board *m_board;
  static Game *m_game;
};

Board *TestBoard::m_board = nullptr;
Game *TestBoard::m_game = nullptr;

TEST_F(TestBoard, ConstructDestruct) {}

TEST_F(TestBoard, PawnLogic) {
  // For white
  auto *whitePawn = m_board->getPieceAt({0, 1});
  EXPECT_TRUE(whitePawn->getLetter() == 'P');
  EXPECT_TRUE(whitePawn->getColor() == Color::white);

  EXPECT_TRUE(whitePawn->isValidMove({0, 2}));
  EXPECT_TRUE(whitePawn->isValidMove({0, 3}));

  whitePawn->setPosition({0, 3});
  EXPECT_TRUE(whitePawn->isValidMove({0, 4}));
  EXPECT_FALSE(whitePawn->isValidMove({0, 5}));

  // For black
  auto *blackPawn = m_board->getPieceAt({0, 6});
  EXPECT_TRUE(blackPawn->getLetter() == 'p');
  EXPECT_TRUE(blackPawn->getColor() == Color::black);

  EXPECT_TRUE(blackPawn->isValidMove({0, 5}));
  EXPECT_TRUE(blackPawn->isValidMove({0, 4}));

  blackPawn->setPosition({0, 4});
  EXPECT_TRUE(blackPawn->isValidMove({0, 3}));
  EXPECT_FALSE(blackPawn->isValidMove({0, 2}));
}

TEST_F(TestBoard, KnightLogic) {
  auto *blackKnight = m_board->getPieceAt({1, 7});
  EXPECT_TRUE(blackKnight->getLetter() == 'n');
  EXPECT_TRUE(blackKnight->getColor() == Color::black);

  EXPECT_TRUE(blackKnight->isValidMove({2, 5}));
  EXPECT_TRUE(blackKnight->isValidMove({0, 5}));

  blackKnight->setPosition({2, 5});
  EXPECT_TRUE(blackKnight->isValidMove({3, 3}));
}

TEST_F(TestBoard, BishopLogic) {
  auto *whiteBishop = m_board->getPieceAt({2, 0});
  EXPECT_TRUE(whiteBishop->getLetter() == 'B');
  EXPECT_TRUE(whiteBishop->getColor() == Color::white);

  EXPECT_TRUE(whiteBishop->isValidMove({1, 1}));
  EXPECT_TRUE(whiteBishop->isValidMove({3, 1}));
  EXPECT_FALSE(whiteBishop->isValidMove({4, 1}));
}

TEST_F(TestBoard, RookLogic) {
  auto *blackRook = m_board->getPieceAt({7, 7});
  EXPECT_TRUE(blackRook->getLetter() == 'r');
  EXPECT_TRUE(blackRook->getColor() == Color::black);

  EXPECT_TRUE(blackRook->isValidMove({7, 0}));
  EXPECT_TRUE(blackRook->isValidMove({0, 7}));
  EXPECT_FALSE(blackRook->isValidMove({6, 6}));
}

TEST_F(TestBoard, QueenLogic) {
  auto *whiteQueen = m_board->getPieceAt({3, 0});
  EXPECT_TRUE(whiteQueen->getLetter() == 'Q');
  EXPECT_TRUE(whiteQueen->getColor() == Color::white);

  EXPECT_TRUE(whiteQueen->isValidMove({2, 1}));
  EXPECT_TRUE(whiteQueen->isValidMove({4, 1}));
  EXPECT_FALSE(whiteQueen->isValidMove({5, 1}));
}

TEST_F(TestBoard, KingLogic) {
  auto *blackKing = m_board->getPieceAt({4, 7});
  EXPECT_TRUE(blackKing->getLetter() == 'k');
  EXPECT_TRUE(blackKing->getColor() == Color::black);

  EXPECT_TRUE(blackKing->isValidMove({4, 6}));
  EXPECT_TRUE(blackKing->isValidMove({3, 6}));
  EXPECT_FALSE(blackKing->isValidMove({3, 5}));
}

TEST_F(TestBoard, HandleMove) {
  // Cheating a bit because we can!
  auto *whitePawn = m_board->getPieceAt({4, 1});
  whitePawn->setPosition({4, 5});
  EXPECT_TRUE(m_board->isValidMove(Color::white, {4, 5}, {5, 6}, true));
}

TEST_F(TestBoard, BasicCastling) {
  m_board->loadFromState(m_game->parseFen(k_testFenFilepath, 0));
  auto *whiteKing = m_board->getPieceAt({4, 0});
  auto *whiteKingsideRook = m_board->getPieceAt({7, 0});
  auto *blackKing = m_board->getPieceAt({4, 7});
  auto *blackQueensideRook = m_board->getPieceAt({0, 7});

  // Both sides are allowed to castle
  EXPECT_TRUE(m_board->getCastleStatus() == k_bothSidesCastleRights);

  // Can white castle on both sides?
  EXPECT_TRUE(m_board->isValidMove(Color::white, {4, 0}, {6, 0}, true));
  EXPECT_TRUE(m_board->isValidMove(Color::white, {4, 0}, {2, 0}, true));

  // Can black castle on both sides?
  EXPECT_TRUE(m_board->isValidMove(Color::black, {4, 7}, {6, 7}, true));
  EXPECT_TRUE(m_board->isValidMove(Color::black, {4, 7}, {2, 7}, true));

  // isValidMove performs the castle
  EXPECT_TRUE(m_board->isValidMove(Color::white, {4, 0}, {6, 0}, false));
  EXPECT_TRUE(whiteKing->getPosition() == Position({6, 0}));
  EXPECT_TRUE(whiteKingsideRook->getPosition() == Position({5, 0}));

  // White can no longer castle
  EXPECT_TRUE(m_board->getCastleStatus() == k_blackOnlyCastleRights);

  EXPECT_TRUE(m_board->isValidMove(Color::black, {4, 7}, {2, 7}, false));
  EXPECT_TRUE(blackKing->getPosition() == Position({2, 7}));
  EXPECT_TRUE(blackQueensideRook->getPosition() == Position({3, 7}));

  // Black can also no longer castle
  EXPECT_TRUE(m_board->getCastleStatus() == k_neitherSideCastleRights);
}

TEST_F(TestBoard, ComplexCastling) {
  m_board->loadFromState(m_game->parseFen(k_testFenFilepath, 1));
  auto *whiteKing = m_board->getPieceAt({4, 0});
  auto *whiteKingsideRook = m_board->getPieceAt({7, 0});
  auto *blackKing = m_board->getPieceAt({4, 7});
  auto *blackQueensideRook = m_board->getPieceAt({0, 7});

  // Both sides are allowed to castle...
  EXPECT_TRUE(m_board->getCastleStatus() == k_bothSidesCastleRights);

  // ...but both are equally indisposed at the current moment
  // White cannot castle kingside: black's rook attacks square in path
  // Black cannot castle queenside: black is in check by white's bishop
  // White cannot castle queenside: there's a piece in the way
  EXPECT_FALSE(m_board->isValidMove(Color::white, {4, 0}, {6, 0}, true));
  EXPECT_FALSE(m_board->isValidMove(Color::white, {4, 0}, {2, 0}, true));
  EXPECT_TRUE(m_board->isKingInCheck(Color::black));
  EXPECT_FALSE(m_board->isValidMove(Color::black, {4, 7}, {6, 7}, true));
  EXPECT_FALSE(m_board->isValidMove(Color::black, {4, 7}, {2, 7}, true));

  // White moves to release black from check
  m_board->movePiece({2, 5}, {6, 1});
  EXPECT_FALSE(m_board->isKingInCheck(Color::black));
  EXPECT_TRUE(m_board->isValidMove(Color::black, {4, 7}, {2, 7}, true));

  // Still, black cannot castle kingside: there's a white pawn attacking
  EXPECT_FALSE(m_board->isValidMove(Color::black, {4, 7}, {6, 7}, true));

  // After moving the black queenside rook,
  // castling is no longer possible on that side...
  m_board->movePiece({0, 7}, {1, 7});
  EXPECT_FALSE(m_board->isValidMove(Color::black, {4, 7}, {2, 7}, true));
  EXPECT_FALSE(m_board->getCastleStatus() == k_bothSidesCastleRights);

  // ...even after moving back
  m_board->movePiece({1, 7}, {0, 7});
  EXPECT_FALSE(m_board->isValidMove(Color::black, {4, 7}, {2, 7}, true));
  EXPECT_TRUE(m_board->getCastleStatus() == k_blackLostQueensideCastleRights);

  // White moving the queen prevents white from ever castling
  m_board->movePiece({4, 0}, {3, 0});
  m_board->updateBoardState({4, 0}, {3, 0});
  EXPECT_TRUE(m_board->getCastleStatus() == k_blackOnlyKingsideCastleRights);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
