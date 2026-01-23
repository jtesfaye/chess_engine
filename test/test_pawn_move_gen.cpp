//
// Created by jeremiah tesfaye on 1/15/26.
//
#include <gtest/gtest.h>
#include <ChessGame.h>
#include <MoveGenerator.h>
#include <algorithm>
class PawnLegalMoves : public ::testing::TestWithParam<std::tuple<std::string, int, Rank, File>> {
protected:
  PawnLegalMoves() = default;
};

static bool contains_move(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST(PawnMovementTest, PawnMoveTest) {
  GameBoard b;
  b.load_from_fen_piece_placement("8/8/8/8/8/8/4P3/8");
  MoveGenerator moves(b);
  const Piece p {Pawn, White};
  const Square s {Rank_2, File_E};

  std::vector<Square> pawn_psuedo_moves = moves.generate_pseudo_legal_moves(s);
  ASSERT_EQ(pawn_psuedo_moves.size(), 2);
  EXPECT_TRUE(contains_move(pawn_psuedo_moves, File::File_E, Rank::Rank_3));
  EXPECT_TRUE(contains_move(pawn_psuedo_moves, File::File_E, Rank::Rank_4));
}

TEST(PawnMovementTest, Pawn_BlockedForward) {
  GameBoard b;
  b.load_from_fen_piece_placement("8/8/8/8/4p3/8/4P3/8");
  MoveGenerator gen(b);
  const Square s{Rank_2, File_E};
  auto moves = gen.generate_pseudo_legal_moves(s);
  ASSERT_EQ(moves.size(), 1);
}

TEST(PawnMovementTest, Pawn_Captures) {
  GameBoard b;
  b.load_from_fen_piece_placement("8/8/8/8/8/3p1p2/4P3/8");
  MoveGenerator gen(b);
  const Square s{Rank_2, File_E};
  auto moves = gen.generate_pseudo_legal_moves(s);
  ASSERT_EQ(moves.size(), 4);
  EXPECT_TRUE(contains_move(moves, File::File_E, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_D, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_F, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_E, Rank::Rank_4));
}

TEST(PawnMovementTest, Pawn_EdgeFile) {
  GameBoard b;
  b.load_from_fen_piece_placement("8/8/8/8/8/8/P7/8");
  MoveGenerator gen(b);
  const Square s{Rank_2, File_A};
  auto moves = gen.generate_pseudo_legal_moves(s);
  ASSERT_EQ(moves.size(), 2);
  EXPECT_TRUE(contains_move(moves, File::File_A, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_A, Rank::Rank_4));
}

TEST_P(PawnLegalMoves, TestPawnLegalMoves) {
  auto [fen_string, expected_val, rank, file] = GetParam();
  ChessGame game(fen_string);
  Piece pawn{Pawn, White};
  Square s{rank, file};
  auto move_list= game.generate_legal_moves(pawn, s);
  ASSERT_EQ(move_list.size(), expected_val);
}

INSTANTIATE_TEST_SUITE_P(
  TestPawnLegalMoves,
  PawnLegalMoves,
   ::testing::Values(
    //Default pawn setup
    std::make_tuple(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      2, Rank_2, File_E),
    //Check if pawn's legal moves are zero when pinned by a bishop
    std::make_tuple(
      "rnbqk1nr/pppppppp/8/8/1b6/8/PPPPPPPP/RNBQKBNR w KQkq d3 0 1",
      0, Rank_2, File_D),
    //This tests if a move the an en passant square is filtered
    std::make_tuple(
      "rnb1kbnr/pp2pppp/3q4/2pP4/P1B2P1P/3KP3/1PP2NP1/RNBQ2R1 w KQkq c6 0 1",
       0, Rank_5, File_D)
    ));