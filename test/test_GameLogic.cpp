#include <algorithm>
#include <gtest/gtest.h>
#include <ChessGame.h>
#include <MoveGenerator.h>

class GameLogicTest : public ::testing::TestWithParam<std::tuple<std::string, Color, bool>> {
protected:
  GameLogicTest() : game(g.get_board()) {}
  ChessGame g;
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
  game.load_from_fen_piece_placement(first);

  ASSERT_EQ(game.to_fen_piece_placement(), first);

}

TEST_F(GameLogicTest, LoadBoard_Orientation) {
  game.load_from_fen_piece_placement("8/8/8/8/8/8/4P3/8");
  auto piece = game.at(Rank_2, File_E);

  EXPECT_EQ(piece.type, PieceType::Pawn);
  EXPECT_EQ(piece.color, Color::White);
}

TEST_F(GameLogicTest, LoadBoard_EdgeSquares) {
  game.load_from_fen_piece_placement("r6k/8/8/8/8/8/8/R6K");
  EXPECT_TRUE(game.at(Rank::Rank_8, File::File_A).type == Rook); // black rook
  EXPECT_TRUE(game.at(Rank::Rank_8, File::File_H).type == King); // black king
  EXPECT_TRUE(game.at(Rank::Rank_1, File::File_A).type == Rook); // white rook
  EXPECT_TRUE(game.at(Rank::Rank_1, File::File_H).type == King); // white king
}

TEST_F(GameLogicTest, LoadBoard_EmptyCompression) {
  game.load_from_fen_piece_placement("3p4/8/8/8/8/8/8/8");
  EXPECT_EQ(game.to_fen_piece_placement(), "3p4/8/8/8/8/8/8/8");
}

TEST_P(GameLogicTest, isCheckTest) {
  auto param = GetParam();
  auto piece_list = game.load_from_fen_piece_placement(std::get<std::string>(param));
  bool res = g.is_check(game, piece_list, std::get<Color>(param));
  EXPECT_EQ(res, std::get<bool>(param));
}

INSTANTIATE_TEST_SUITE_P(
  isCheckTests,
  GameLogicTest,
  ::testing::Values(
    std::make_tuple("4k3/8/8/8/8/8/4R3/4K3", Color::Black, true),  //White Rook put Black King in check
    std::make_tuple("4k3/8/8/8/8/8/4R3/4K3", Color::White, false),
    std::make_tuple("4k3/8/2B5/8/8/8/8/4K3", Color::Black, true),  //White bishop put Black king in check
    std::make_tuple("4k3/8/8/2B5/8/8/8/4K3", Color::White, false),
    std::make_tuple("4k3/8/8/8/8/8/4Q3/4K3", Color::Black, true),  // White Queen put Black king in check
    std::make_tuple("4k3/8/5N2/8/8/8/8/4K3", Color::Black, true),  // White Knight put Black king in check
    std::make_tuple("4k3/8/8/6N1/8/8/8/4K3", Color::Black, false), // White knight did not put black king in check
    std::make_tuple("4k3/8/8/8/3P4/8/8/4K3", Color::Black, false), // King not in check by pawn
    std::make_tuple("4k3/8/8/8/8/8/3p4/4K3", Color::White, true),  // White King is in check by black pawn
    std::make_tuple("4k3/8/8/8/8/8/4P3/4R3", Color::Black, false), //Test if rook is blocked by pawn
    std::make_tuple("7k/8/8/8/3P4/2B5/8/4K3", Color::Black, false), //Test if Bishop blocked by pawn
    std::make_tuple("4k3/8/2B5/8/8/8/4R3/4K3", Color::Black, true), //Test when bishop and rook put black king in check
    std::make_tuple("8/8/8/8/8/8/4k3/4K3", Color::White, true), //Test if king can put king in check
    std::make_tuple("8/8/8/8/8/8/4k3/4K3", Color::Black, true),
    std::make_tuple("4k3/8/8/8/8/8/8/4K3", Color::White, false), //Test if no check is detected
    std::make_tuple("4k3/8/8/8/8/8/8/4K3", Color::Black, false)
    ));


