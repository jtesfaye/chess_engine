#include <algorithm>
#include <gtest/gtest.h>
#include <GameLogic.h>
#include <MoveGenerator.h>

class GameLogicTest : public ::testing::Test {
protected:
  GameLogicTest() : game(g.get_board()) {}
  GameLogic g;
  GameBoard& game;
};

static bool contains_move(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST_F(GameLogicTest, LoadBoardTest) {
  std::string first = "8/8/8/8/8/8/4P3/8";
  game.load_board(first);

  ASSERT_EQ(game.to_fen_piece_placement(), first);

}

TEST_F(GameLogicTest, LoadBoard_Orientation) {
  game.load_board("8/8/8/8/8/8/4P3/8");
  auto piece = game.at(Rank_2, File_E);

  EXPECT_EQ(piece.type, PieceType::Pawn);
  EXPECT_EQ(piece.color, Color::White);
}

TEST_F(GameLogicTest, LoadBoard_EdgeSquares) {
  game.load_board("r6k/8/8/8/8/8/8/R6K");
  EXPECT_TRUE(game.at(Rank::Rank_8, File::File_A).type == Rook); // black rook
  EXPECT_TRUE(game.at(Rank::Rank_8, File::File_H).type == King); // black king
  EXPECT_TRUE(game.at(Rank::Rank_1, File::File_A).type == Rook); // white rook
  EXPECT_TRUE(game.at(Rank::Rank_1, File::File_H).type == King); // white king
}

TEST_F(GameLogicTest, LoadBoard_EmptyCompression) {
  game.load_board("3p4/8/8/8/8/8/8/8");
  EXPECT_EQ(game.to_fen_piece_placement(), "3p4/8/8/8/8/8/8/8");
}

