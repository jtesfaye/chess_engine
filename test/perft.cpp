//
// Created by jeremiah tesfaye on 1/20/26.
//

#include <gtest/gtest.h>
#include <ChessGame.h>
#include <chrono>
static size_t run_game(ChessGame& game, int depth, Color turn) {
  if (depth == 0) {
    return 1;
  }
  size_t nodes{};
  const Color next_turn = turn == White ? Black : White;
  std::vector<Position> piece_list = game.get_piece_list();
  for (const auto [piece, square] : piece_list) {
    if (piece.color != turn) {
      continue;
    }
    std::vector<Move> moves = std::move(game.generate_legal_moves(piece, square));
    for (const auto m : moves) {
      game.apply_move(m);
      nodes += run_game(game, depth - 1, next_turn);
      game.undo_move();
    }
  }
  return nodes;
}

static size_t perft_divide(ChessGame& game, int depth, Color turn) {
  size_t total = 0;
  const Color next_turn = (turn == White) ? Black : White;

  auto piece_list = game.get_piece_list();
  for (const auto& [piece, square] : piece_list) {
    if (piece.color != turn) {
      continue;
    }
    auto moves = game.generate_legal_moves(piece, square);
    for (const auto& m : moves) {
      game.apply_move(m);
      size_t count = run_game(game, depth - 1, next_turn);
      total += count;
      std::cout << m.from.to_string() << ":" << m.to.to_string() << " " << count << std::endl;
      game.undo_move();
    }
  }

  return total;
}
TEST(PerfTest, Depth1) {
  ChessGame game;
  int depth = 5;
  auto start = std::chrono::system_clock::now();
  std::cout << run_game(game, depth, White);
  auto end = std::chrono::system_clock::now();
  auto time = end - start;
  std::cout << "Time taken sec: " << std::chrono::duration_cast<std::chrono::seconds>(time).count() << std::endl;
  std::cout << "Time taken min: " << std::chrono::duration_cast<std::chrono::minutes>(time).count() << std::endl;
}
