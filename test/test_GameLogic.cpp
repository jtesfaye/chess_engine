#include <gtest/gtest.h>
#include <GameLogic.h>
#include <MoveGenerator.h>

class GameLogicTest : public ::testing::Test {
protected:
  GameLogicTest() = default;
  GameLogic game;
};

TEST_F(GameLogicTest, PawnMoveTest) {
  auto b = game.get_board();
  std::vector<MoveChange> changes{};
  MoveGenerator moves(b, changes);
  Position p{File::File_E, Rank::Rank_4, Piece{PieceType::Pawn, Color::White}};

  MoveList pawn_psuedo_moves = moves.generate_pseudo_legal_moves(p);

  ASSERT_EQ(pawn_psuedo_moves.size(), 2);

}