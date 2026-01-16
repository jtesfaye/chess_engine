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