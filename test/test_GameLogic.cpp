#include <algorithm>
#include <gtest/gtest.h>
#include <ChessGame.h>
#include <MoveGenerator.h>
#include <iostream>

class GameLogicTest : public ::testing::TestWithParam<std::tuple<std::string, bool>> {
protected:
  GameLogicTest() = default;
};

class ChessGameTest : public ::testing::TestWithParam<std::string> {
protected:
  ChessGameTest() = default;
};

static bool contains_move(const std::vector<Square>& moves, size_t file, size_t rank) {
  return std::any_of(moves.begin(), moves.end(),
      [&](const Square& m) {
          return m.file == file && m.rank == rank;
      });
}

TEST_F(GameLogicTest, LoadBoardTest) {
  std::string first = "8/8/8/8/8/8/4P3/8";
  GameBoard game(first);
  ASSERT_EQ(game.to_fen_piece_placement(), first);
}

TEST_P(ChessGameTest, ConstructChessGameFromFen) {
  auto fen_str = GetParam();
  ASSERT_NO_THROW(ChessGame{fen_str});
}

TEST_P(GameLogicTest, isCheckTest) {
  const auto[fen, res] = GetParam();
  ChessGame game(fen);
  auto piece_list = game.get_piece_list();
  for (const auto [piece, square] : piece_list) {
    if (piece.color == game.get_current_turn() && piece.type == King) {
      std::cout << piece.color << std::endl;
      std::cout << square.rank << " " << square.file << std::endl;
      EXPECT_EQ(game.is_check(square), res);
      break;
    }
  }
}

INSTANTIATE_TEST_SUITE_P(canConstructFromFen, ChessGameTest, ::testing::Values(
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
  "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 2 3",
  "8/8/8/8/8/8/4K3/4k3 w - - 0 1",
  "8/8/8/8/8/8/8/4K3 b - - 15 42",
  "4k3/8/8/8/8/8/8/4K3 w - - 0 1",
  "r3k2r/8/8/8/8/8/8/R3K2R w KQ - 0 1",
  "r3k2r/8/8/8/8/8/8/R3K2R b kq - 0 1",
  "8/8/8/8/8/8/8/4K3 w - - 99 999"));

INSTANTIATE_TEST_SUITE_P(
  isCheckTests,
  GameLogicTest,
  ::testing::Values(
    std::make_tuple("4k3/8/8/8/8/8/4R3/4K3 b KQkq - 0 1", true),  //White Rook put Black King in check
    std::make_tuple("4k3/8/8/8/8/8/4R3/4K3 w KQkq - 0 1", false),
    std::make_tuple("4k3/8/2B5/8/8/8/8/4K3 b KQkq - 0 1", true),  //White bishop put Black king in check
    std::make_tuple("4k3/8/8/2B5/8/8/8/4K3 w KQkq - 0 1", false),
    std::make_tuple("4k3/8/8/8/8/8/4Q3/4K3 b KQkq - 0 1", true),  // White Queen put Black king in check
    std::make_tuple("4k3/8/5N2/8/8/8/8/4K3 b KQkq - 0 1", true),  // White Knight put Black king in check
    std::make_tuple("4k3/8/8/6N1/8/8/8/4K3 b KQkq - 0 1", false), // White knight did not put black king in check
    std::make_tuple("4k3/8/8/8/3P4/8/8/4K3 b KQkq - 0 1", false), // King not in check by pawn
    std::make_tuple("4k3/8/8/8/8/8/3p4/4K3 w KQkq - 0 1", true),  // White King is in check by black pawn
    std::make_tuple("4k3/8/8/8/8/8/4P3/4R3 b KQkq - 0 1", false), //Test if rook is blocked by pawn
    std::make_tuple("7k/8/8/8/3P4/2B5/8/4K3 b KQkq - 0 1", false), //Test if Bishop blocked by pawn
    std::make_tuple("4k3/8/2B5/8/8/8/4R3/4K3 b KQkq - 0 1", true), //Test when bishop and rook put black king in check
    std::make_tuple("8/8/8/8/8/8/4k3/4K3 w KQkq - 0 1", true), //Test if king can put king in check
    std::make_tuple("8/8/8/8/8/8/4k3/4K3 b KQkq - 0 1", true),
    std::make_tuple("4k3/8/8/8/8/8/8/4K3 w KQkq - 0 1", false), //Test if no check is detected
    std::make_tuple("4k3/8/8/8/8/8/8/4K3 b KQkq - 0 1", false)
    ));




