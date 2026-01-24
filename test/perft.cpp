//
// Created by jeremiah tesfaye on 1/20/26.
//

#include <gtest/gtest.h>
#include <ChessGame.h>

static size_t run_game(ChessGame& game, int depth, Color turn) {
  if (depth == 0) {
    return 0;
  }
  const Color next_turn = turn == White ? Black : White;
  std::vector<Position> piece_list = game.get_piece_list();
  size_t total_moves{};
  for (const auto [piece, square] : piece_list) {
    if (piece.color == turn) {
      std::vector<Move> moves = std::move(game.generate_legal_moves(piece, square));
      for (const auto m : moves) {
        game.apply_move(m);
        const size_t res = run_game(game, depth - 1, next_turn);
        game.undo_move();
        total_moves += res;
      }
      total_moves += moves.size();
    }
  }
  return total_moves;
}
TEST(PerfTest, Depth1) {
  ChessGame game;
  size_t res = run_game(game, 2, White);
  std::cout << "Total moves: " << res << std::endl;
}
