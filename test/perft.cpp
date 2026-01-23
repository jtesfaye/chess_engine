//
// Created by jeremiah tesfaye on 1/20/26.
//

#include <gtest/gtest.h>
#include <ChessGame.h>

static void run_game(ChessGame& game, int depth) {
  if (depth == 0) {
    return;
  }
  std::vector<Position> piece_list = game.get_piece_list();
  size_t total_moves{};
  for (const auto [piece, square] : piece_list) {
    std::vector<Move> moves = std::move(game.generate_legal_moves(piece, square));
    for (const auto m : moves) {
      game.apply_move(m);
      run_game(game, depth - 1);
      game.undo_move();
    }
    total_moves += moves.size();
  }
  std::cout << total_moves << std::endl;
}
TEST(PerfTest, Depth1) {
  ChessGame game;
  run_game(game, 1);
}
