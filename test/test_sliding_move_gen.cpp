//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <gtest/gtest.h>
#include <GameLogic.h>
#include <MoveGenerator.h>
#include <algorithm>

class SlidingMoveTest : public ::testing::Test {
protected:
  SlidingMoveTest() : board(logic.get_board()) {}
  GameLogic logic;
  GameBoard& board;
};

static bool contains(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}
/*
 * Bishop tests
 */
TEST_F(SlidingMoveTest, BishopCenterMoves) {
  board.load_board("8/8/8/3B4/8/8/8/8");
  auto board = logic.get_board();
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Bishop, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  ASSERT_EQ(moves.size(), 13);
  // NE
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_7));
  EXPECT_TRUE(contains(moves, File::File_G, Rank::Rank_8));
  // NW
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_7));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_8));
  // SE
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_G, Rank::Rank_2));
  EXPECT_TRUE(contains(moves, File::File_H, Rank::Rank_1));
  // SW
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_2));
}

TEST_F(SlidingMoveTest, BishopCornerMoves) {
  board.load_board("8/8/8/8/8/8/8/B7");
  auto board = logic.get_board();
  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_A,
    Piece{Bishop, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  ASSERT_EQ(moves.size(), 7);
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_2));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_G, Rank::Rank_7));
  EXPECT_TRUE(contains(moves, File::File_H, Rank::Rank_8));
}

TEST_F(SlidingMoveTest, BishopBlockedByFriendly) {
  board.load_board("8/8/8/3B4/4P3/8/8/8");
  auto board = logic.get_board();
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Bishop, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_FALSE(contains(moves, File::File_E, Rank::Rank_4));
}

TEST_F(SlidingMoveTest, BishopCapturesEnemyStops) {
  board.load_board("8/8/8/3B4/4p3/8/8/8");
  auto board = logic.get_board();
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Bishop, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_4));   // capture
  EXPECT_FALSE(contains(moves, File::File_F, Rank::Rank_3));  // beyond capture
}

TEST_F(SlidingMoveTest, BishopFullyBlocked) {
  board.load_board("8/8/2P1P3/2PBP3/2P1P3/8/8/8");
  auto board = logic.get_board();
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Bishop, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_TRUE(moves.empty());
}

TEST_F(SlidingMoveTest, BishopOnlyMovesDiagonally) {
  board.load_board("8/8/8/3B4/8/8/8/8");
  auto board = logic.get_board();
  MoveGenerator gen(board, {});
  Position p{
    Rank_4,
    File_D,
    Piece{Bishop, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_FALSE(contains(moves, File::File_D, Rank::Rank_5));
  EXPECT_FALSE(contains(moves, File::File_E, Rank::Rank_4));
}

/*
 * Rook Tests
 */
TEST_F(SlidingMoveTest, RookCenterMoves) {
  board.load_board("8/8/8/3R4/8/8/8/8");
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Rook, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  ASSERT_EQ(moves.size(), 14);
  // North
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_7));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_8));
  // South
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_2));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_1));
  // East
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_G, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_H, Rank::Rank_5));
  // West
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_5));
}

TEST_F(SlidingMoveTest, RookCornerMoves) {
  board.load_board("8/8/8/8/8/8/8/R7");
  MoveGenerator gen(board, {});
  Position p{
    Rank_1,
    File_A,
    Piece{Rook, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  ASSERT_EQ(moves.size(), 14);

  // Rank 1
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_1));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_1));
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_1));
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_1));
  EXPECT_TRUE(contains(moves, File::File_F, Rank::Rank_1));
  EXPECT_TRUE(contains(moves, File::File_G, Rank::Rank_1));
  EXPECT_TRUE(contains(moves, File::File_H, Rank::Rank_1));

  // File A
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_2));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_7));
  EXPECT_TRUE(contains(moves, File::File_A, Rank::Rank_8));
}

TEST_F(SlidingMoveTest, RookBlockedByFriendly) {
  board.load_board("8/8/8/3R4/3P4/8/8/8");
  MoveGenerator gen(board, {});
  Piece p{
    Rook,
    White,
    Rank_5,
    File_D,
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_FALSE(contains(moves, File::File_D, Rank::Rank_4)); // blocked
}

TEST_F(SlidingMoveTest, RookCapturesEnemyStops) {
  board.load_board("8/8/8/3R4/3p4/8/8/8");
  MoveGenerator gen(board, {});
  Piece p{
    Rook,
    White,
    Rank_5,
    File_D,
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_4));   // capture
  EXPECT_FALSE(contains(moves, File::File_D, Rank::Rank_3));  // cannot pass through
}

TEST_F(SlidingMoveTest, RookFullyBlocked) {
  board.load_board("8/8/8/3P4/2PRP3/3P4/8/8");
  MoveGenerator gen(board, {});
  Position p{
    Rank_4,
    File_D,
    Piece{Rook, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_TRUE(moves.empty());
}

TEST_F(SlidingMoveTest, RookOnlyMovesOrthogonally) {
  board.load_board("8/8/8/3R4/8/8/8/8");
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Rook, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_FALSE(contains(moves, File::File_E, Rank::Rank_6));
  EXPECT_FALSE(contains(moves, File::File_C, Rank::Rank_4));
}

/*
 * Queen Tests
*/
TEST_F(SlidingMoveTest, QueenStopsAfterCaptureAllDirections) {
  board.load_board("8/8/4p3/3Q4/8/8/8/8");
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Queen, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  ASSERT_EQ(moves.size(), 25);
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_6));
  EXPECT_FALSE(contains(moves, File::File_F, Rank::Rank_7));
}

TEST_F(SlidingMoveTest, QueenMovesOrthogonalAndDiagonal) {
  board.load_board("8/8/8/3Q4/8/8/8/8");
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Queen, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  EXPECT_TRUE(contains(moves, File::File_D, Rank::Rank_8));
  EXPECT_TRUE(contains(moves, File::File_H, Rank::Rank_1));
}

TEST_F(SlidingMoveTest, QueenCenterMoves) {
  board.load_board("8/8/8/3Q4/8/8/8/8");
  MoveGenerator gen(board, {});
  Position p{
    Rank_5,
    File_D,
    Piece{Queen, White}
  };
  auto moves = gen.generate_pseudo_legal_moves(p);
  // rook (14) + bishop (13)
  EXPECT_EQ(moves.size(), 27);
}
