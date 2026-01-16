//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <gtest/gtest.h>
#include <GameLogic.h>
#include <MoveGenerator.h>
#include <algorithm>

class KnightMoveTest : public ::testing::Test {
protected:
  KnightMoveTest() : logic(l.get_board()) {}
  GameLogic l;
  GameBoard& logic;
};

using Move = std::pair<size_t, size_t>;

static bool contains(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST_F(KnightMoveTest, KnightCenterHasEightMoves) {
  logic.load_board("8/8/8/3N4/8/8/8/8");
  auto& board = logic;
  MoveGenerator gen(board, {});
  Position p{
    Rank_4,
    File_D,
    Piece{Knight, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);

  ASSERT_EQ(moves.size(), 8);
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_2));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_2));
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_5));
}

TEST_F(KnightMoveTest, KnightCornerHasTwoMoves) {
  logic.load_board("8/8/8/8/8/8/8/N7");
  auto& board = logic;
  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_A,
    Piece{Knight, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);

  ASSERT_EQ(moves.size(), 2);
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_2));
}

TEST_F(KnightMoveTest, KnightEdgeHasFourMoves) {
  logic.load_board("8/8/8/N7/8/8/8/8");
  auto board = logic;
  MoveGenerator gen(board, {});
  Position p{
    Rank_4,
    File_A,
    Piece{Knight, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p);

  ASSERT_EQ(moves.size(), 4);
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_2));
}

TEST_F(KnightMoveTest, KnightCannotCaptureOwnPiece) {
  logic.load_board("8/8/8/3N4/2P1P3/8/8/8");
  auto board = logic;
  MoveGenerator gen(board, {});
  Position p{
    Rank_4,
    File_D,
    Piece{Knight, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);

  EXPECT_FALSE(contains(moves, File::File_C, Rank::Rank_3));
  EXPECT_FALSE(contains(moves, File::File_E, Rank::Rank_3));
}

TEST_F(KnightMoveTest, KnightCanCaptureEnemy) {
  logic.load_board("8/8/3N4/8/2p1p3/8/8/8");
  auto board = logic;
  MoveGenerator gen(board, {});
  Position p{
    Rank_6,
    File_D,
    Piece{Knight, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p);

  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_4));
}

TEST_F(KnightMoveTest, KnightJumpsOverPieces) {
  logic.load_board("8/8/8/3N4/3PPP2/8/8/8");
  auto board = logic;
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Knight, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  ASSERT_EQ(moves.size(), 7);
}