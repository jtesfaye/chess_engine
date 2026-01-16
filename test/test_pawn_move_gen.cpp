//
// Created by jeremiah tesfaye on 1/15/26.
//
#include <gtest/gtest.h>
#include <GameLogic.h>
#include <MoveGenerator.h>
#include <algorithm>

class GameLogicTest : public ::testing::Test {
protected:
  GameLogicTest() : board(g.get_board()) {}
  GameLogic g{};
  GameBoard& board;
};

static bool contains_move(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST_F(GameLogicTest, PawnMoveTest) {
  auto b = board;
  std::vector<MoveChange> changes{};
  MoveGenerator moves(b, changes);
  const Position p{Rank::Rank_2, File_E,  Piece{Pawn, White}};

  std::vector<Square> pawn_psuedo_moves = moves.generate_pseudo_legal_moves(p);
  ASSERT_EQ(pawn_psuedo_moves.size(), 2);
  EXPECT_TRUE(contains_move(pawn_psuedo_moves, File::File_E, Rank::Rank_3));
  EXPECT_TRUE(contains_move(pawn_psuedo_moves, File::File_E, Rank::Rank_4));
}

TEST_F(GameLogicTest, Pawn_BlockedForward) {
  board.load_board("8/8/8/8/4p3/8/4P3/8");

  auto b = board;
  std::vector<MoveChange> changes;
  MoveGenerator gen(b, changes);

  Position p{Rank::Rank_2, File_E, Piece{Pawn, White}};
  auto moves = gen.generate_pseudo_legal_moves(p);

  ASSERT_EQ(moves.size(), 1);
}

TEST_F(GameLogicTest, Pawn_Captures) {
  board.load_board("8/8/8/8/8/3p1p2/4P3/8");

  auto b = board;
  std::vector<MoveChange> changes;
  MoveGenerator gen(b, changes);

  Position p{Rank_2, File_E, Piece{Pawn, White}};
  auto moves = gen.generate_pseudo_legal_moves(p);

  ASSERT_EQ(moves.size(), 4);
  EXPECT_TRUE(contains_move(moves, File::File_E, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_D, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_F, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_E, Rank::Rank_4));
}

TEST_F(GameLogicTest, Pawn_EdgeFile) {
  board.load_board("8/8/8/8/8/8/P7/8");

  auto b = board;
  std::vector<MoveChange> changes;
  MoveGenerator gen(b, changes);

  Position p{Rank_2, File_A, Piece{Pawn, White}};
  auto moves = gen.generate_pseudo_legal_moves(p);

  ASSERT_EQ(moves.size(), 2);
  EXPECT_TRUE(contains_move(moves, File::File_A, Rank::Rank_3));
  EXPECT_TRUE(contains_move(moves, File::File_A, Rank::Rank_4));
}

TEST_F(GameLogicTest, EnPassantTest) {

  Position p1{Rank_5, File_D, Pawn, White};
  Position p2{Rank_4, File_D, Pawn, Black};
  Position p3{Rank_7, File_E, Pawn, White};


  MoveChange change{
    Position{Rank_7, File_E, Pawn, Black},
    Position{Rank_5, File_E, Pawn, Black}
  };

  MoveChange change2{
    Position{Rank_2, File_C, Pawn, White},
    Position{Rank_4, File_C, Pawn, White}
  };

  ASSERT_TRUE(MoveGenerator::canEnPassant(p1, change));
  ASSERT_TRUE(MoveGenerator::canEnPassant(p2, change2));
  ASSERT_FALSE(MoveGenerator::canEnPassant(p3, change2));
}