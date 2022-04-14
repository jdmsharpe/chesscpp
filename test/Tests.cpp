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
    auto* whiteKnight = m_board->getPieceAt({1, 0});
    EXPECT_TRUE(whiteKnight->getLetter() == 'n');
    EXPECT_TRUE(whiteKnight->getColor() == Color::white);

    EXPECT_TRUE(whiteKnight->isValidMove({2, 2}));
    EXPECT_TRUE(whiteKnight->isValidMove({0, 2}));

    whiteKnight->setPosition({2, 2});
    EXPECT_TRUE(whiteKnight->isValidMove({3, 4}));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}