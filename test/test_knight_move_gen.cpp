//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <gtest/gtest.h>
#include <ChessGame.h>
#include <MoveGenerator.h>
#include <algorithm>

class KnightMoveTest : public ::testing::Test {
protected:
  KnightMoveTest() {}
  GameBoard board;
};

class KnightLegalMoves : public ::testing::TestWithParam<std::tuple<std::string, int, Rank, File, Color>> {
protected:
  KnightLegalMoves() = default;
};

static bool contains(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST_F(KnightMoveTest, KnightCenterHasEightMoves) {

  board.load_from_fen_piece_placement("8/8/8/8/3N4/8/8/8");
  MoveGenerator gen(board);
  Square s {Rank_4, File_D};
  auto moves = gen.generate_pseudo_legal_moves(s);
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
  board.load_from_fen_piece_placement("8/8/8/8/8/8/8/N7");
  MoveGenerator gen(board);
  Square s{Rank_1, File_A};
  auto moves = gen.generate_pseudo_legal_moves(s);
  ASSERT_EQ(moves.size(), 2);
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_2));
}

TEST_F(KnightMoveTest, KnightEdgeHasFourMoves) {
  board.load_from_fen_piece_placement("8/8/8/8/N7/8/8/8");
  MoveGenerator gen(board);
  Square s{Rank_4, File_A};
  auto moves = gen.generate_pseudo_legal_moves(s);
  ASSERT_EQ(moves.size(), 4);
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_6));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_5));
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_3));
  EXPECT_TRUE(contains(moves, File::File_B, Rank::Rank_2));
}

TEST_F(KnightMoveTest, KnightCannotCaptureOwnPiece) {
  board.load_from_fen_piece_placement("8/8/8/3N4/8/2P1P3/8/8");
  MoveGenerator gen(board);
  Square s {Rank_5, File_D};
  auto moves = gen.generate_pseudo_legal_moves(s);
  EXPECT_FALSE(contains(moves, File::File_C, Rank::Rank_3));
  EXPECT_FALSE(contains(moves, File::File_E, Rank::Rank_3));
}

TEST_F(KnightMoveTest, KnightCanCaptureEnemy) {
  board.load_from_fen_piece_placement("8/8/3N4/8/2p1p3/8/8/8");
  MoveGenerator gen(board);
  Square s{Rank_6, File_D};
  auto moves = gen.generate_pseudo_legal_moves(s);
  EXPECT_TRUE(contains(moves, File::File_C, Rank::Rank_4));
  EXPECT_TRUE(contains(moves, File::File_E, Rank::Rank_4));
}

TEST_F(KnightMoveTest, KnightJumpsOverPieces) {
  board.load_from_fen_piece_placement("8/8/8/3N4/3PPP2/8/8/8");
  MoveGenerator gen(board);
  Square s {Rank_5, File_D};
  auto moves = gen.generate_pseudo_legal_moves(s);
  ASSERT_EQ(moves.size(), 7);
}

TEST_P(KnightLegalMoves, TestKnightLegalMoves) {
  auto [fen_string, expected_val, rank, file, color] = GetParam();
  ChessGame game(fen_string);
  Piece pawn{Knight, color};
  Square s{rank, file};
  auto move_list= game.generate_legal_moves(pawn, s);
  ASSERT_EQ(move_list.size(), expected_val);
}

INSTANTIATE_TEST_SUITE_P(
  TestKnightLegalMoves,
  KnightLegalMoves,
   ::testing::Values(
    //Black queen pins knight
    std::make_tuple(
      "rnb1kbnr/pppppppp/8/8/4q3/4N3/PPPP1PPP/RNBQKB1R w KQkq - 0 1",
      0, Rank_3, File_E, White),
    //Check if pawn's legal moves are zero when pinned by a bishop
    std::make_tuple(
      "rn1qkbnr/pppppppp/8/8/1b1P4/2N1N3/PPP2PPP/R1BQKB1R w KQkq - 0 1",
      0, Rank_3, File_C, White),
    //This tests only 2 move is returned when king is in check, either captuing the bishop or blocking it.
    std::make_tuple(
      "rn1qkbnr/pppppppp/8/3N4/1b1P4/4N3/PPP2PPP/R1BQKB1R w KQkq - 0 1 w KQkq - 0 1",
       2, Rank_5, File_D, White)
    ));