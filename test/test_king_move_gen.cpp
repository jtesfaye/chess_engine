//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <gtest/gtest.h>
#include <GameLogic.h>
#include <MoveGenerator.h>
#include <algorithm>

class KingMoveTest : public ::testing::Test {
protected:
  KingMoveTest() : board(logic.get_board()) {}
  GameLogic logic;
  GameBoard& board;
};

static bool contains(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST_F(KingMoveTest, KingCenterMoves) {
  board.load_board("8/8/8/3K4/8/8/8/8");

  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, false, false);

  ASSERT_EQ(moves.size(), 8);

  EXPECT_TRUE(contains(moves, File_D, Rank_6));
  EXPECT_TRUE(contains(moves, File_D, Rank_4));
  EXPECT_TRUE(contains(moves, File_E, Rank_5));
  EXPECT_TRUE(contains(moves, File_C, Rank_5));

  EXPECT_TRUE(contains(moves, File_E, Rank_6));
  EXPECT_TRUE(contains(moves, File_C, Rank_6));
  EXPECT_TRUE(contains(moves, File_E, Rank_4));
  EXPECT_TRUE(contains(moves, File_C, Rank_4));
}

TEST_F(KingMoveTest, KingCornerMoves) {
  board.load_board("8/8/8/8/8/8/8/K7");

  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_A,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, false, false);

  ASSERT_EQ(moves.size(), 3);

  EXPECT_TRUE(contains(moves, File_A, Rank_2));
  EXPECT_TRUE(contains(moves, File_B, Rank_1));
  EXPECT_TRUE(contains(moves, File_B, Rank_2));
}

TEST_F(KingMoveTest, KingBlockedByFriendly) {
  board.load_board("8/8/8/3K4/3P4/8/8/8");

  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, false, false);
  EXPECT_FALSE(contains(moves, File_D, Rank_4));
}

TEST_F(KingMoveTest, KingCapturesEnemy) {
  board.load_board("8/8/8/3K4/3p4/8/8/8");

  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, false, false);

  EXPECT_TRUE(contains(moves, File_D, Rank_4));
}

TEST_F(KingMoveTest, KingDoesNotJump) {
  board.load_board("8/8/8/8/3P4/3K4/8/8");

  MoveGenerator gen(board, {});
  Position p{
    Rank_2,
    File_D,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, false, false);

  EXPECT_FALSE(contains(moves, File_D, Rank_4));
}

TEST_F(KingMoveTest, KingKingsideCastlingFlagAddsMove) {
  board.load_board("8/8/8/8/8/8/8/4K3");

  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_E,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, true, false);

  EXPECT_TRUE(contains(moves, File_G, Rank_1));
}

TEST_F(KingMoveTest, KingQueensideCastlingFlagAddsMove) {
  board.load_board("8/8/8/8/8/8/8/4K3");

  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_E,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, false, true);

  EXPECT_TRUE(contains(moves, File_C, Rank_1));
}

TEST_F(KingMoveTest, KingBothCastlingFlags) {
  board.load_board("8/8/8/8/8/8/8/4K3");

  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_E,
    Piece{King, White}
  };

  auto moves = gen.generate_pseudo_legal_moves(p, true, true);

  EXPECT_TRUE(contains(moves, File_G, Rank_1));
  EXPECT_TRUE(contains(moves, File_C, Rank_1));
}