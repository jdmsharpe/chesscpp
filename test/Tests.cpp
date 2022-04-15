#include "Board.h"

#include <gtest/gtest.h>

class TestBoard : public ::testing::Test
{
public:
    TestBoard() {}

    static void SetUpTestSuite()
    {
        if (m_board == nullptr)
        {
            m_board = new Board();
        }
    }

    static void TearDownTestSuite()
    {
        delete m_board;
        m_board = nullptr;
    }

    void SetUp() override {}
    void TearDown() override {}

    static Board *m_board;
};

Board* TestBoard::m_board = nullptr;

TEST_F(TestBoard, ConstructDestruct)
{
}

TEST_F(TestBoard, PawnLogic) {
    // For white
    auto* whitePawn = m_board->getPieceAt({0, 1});
    EXPECT_TRUE(whitePawn->getLetter() == 'p');
    EXPECT_TRUE(whitePawn->getColor() == Color::white);

    EXPECT_TRUE(whitePawn->isValidMove({0, 2}));
    EXPECT_TRUE(whitePawn->isValidMove({0, 3}));

    whitePawn->setPosition({0, 3});
    EXPECT_TRUE(whitePawn->isValidMove({0, 4}));
    EXPECT_FALSE(whitePawn->isValidMove({0, 5}));

    // For black
    auto* blackPawn = m_board->getPieceAt({0, 6});
    EXPECT_TRUE(blackPawn->getLetter() == 'p');
    EXPECT_TRUE(blackPawn->getColor() == Color::black);

    EXPECT_TRUE(blackPawn->isValidMove({0, 5}));
    EXPECT_TRUE(blackPawn->isValidMove({0, 4}));

    blackPawn->setPosition({0, 4});
    EXPECT_TRUE(blackPawn->isValidMove({0, 3}));
    EXPECT_FALSE(blackPawn->isValidMove({0, 2}));
}

TEST_F(TestBoard, KnightLogic) {
    auto* blackKnight = m_board->getPieceAt({1, 7});
    EXPECT_TRUE(blackKnight->getLetter() == 'n');
    EXPECT_TRUE(blackKnight->getColor() == Color::black);

    EXPECT_TRUE(blackKnight->isValidMove({2, 5}));
    EXPECT_TRUE(blackKnight->isValidMove({0, 5}));

    blackKnight->setPosition({2, 5});
    EXPECT_TRUE(blackKnight->isValidMove({3, 3}));
}

TEST_F(TestBoard, BishopLogic) {
    auto* whiteBishop = m_board->getPieceAt({2, 0});
    EXPECT_TRUE(whiteBishop->getLetter() == 'b');
    EXPECT_TRUE(whiteBishop->getColor() == Color::white);

    EXPECT_TRUE(whiteBishop->isValidMove({1, 1}));
    EXPECT_TRUE(whiteBishop->isValidMove({3, 1}));
    EXPECT_FALSE(whiteBishop->isValidMove({4, 1}));
}

TEST_F(TestBoard, RookLogic) {
    auto* blackRook = m_board->getPieceAt({7, 7});
    EXPECT_TRUE(blackRook->getLetter() == 'r');
    EXPECT_TRUE(blackRook->getColor() == Color::black);

    EXPECT_TRUE(blackRook->isValidMove({7, 0}));
    EXPECT_TRUE(blackRook->isValidMove({0, 7}));
    EXPECT_FALSE(blackRook->isValidMove({6, 6}));
}

TEST_F(TestBoard, QueenLogic) {
    auto* whiteQueen = m_board->getPieceAt({3, 0});
    EXPECT_TRUE(whiteQueen->getLetter() == 'q');
    EXPECT_TRUE(whiteQueen->getColor() == Color::white);

    EXPECT_TRUE(whiteQueen->isValidMove({2, 1}));
    EXPECT_TRUE(whiteQueen->isValidMove({4, 1}));
    EXPECT_FALSE(whiteQueen->isValidMove({5, 1}));
}

TEST_F(TestBoard, KingLogic) {
    auto* blackKing = m_board->getPieceAt({4, 7});
    EXPECT_TRUE(blackKing->getLetter() == 'k');
    EXPECT_TRUE(blackKing->getColor() == Color::black);

    EXPECT_TRUE(blackKing->isValidMove({4, 6}));
    EXPECT_TRUE(blackKing->isValidMove({3, 6}));
    EXPECT_FALSE(blackKing->isValidMove({3, 5}));
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
