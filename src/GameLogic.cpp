#include <GameLogic.h>
#include <util.h>
#include <cassert>
#include <GameTypes.h>
#include <sstream>

GameLogic::GameLogic() {
  move_history.reserve(1024);
  set_initial_board();
}

void GameLogic::set_initial_board() {

  for (size_t i = File_A; i <= File_H; i++) {
    board.set_piece(Rank_2, static_cast<File>(i), {Pawn, White});
    board.set_piece(Rank_7, static_cast<File>(i), {Pawn, Black});
  }

  board.set_piece(Rank_1, File_A, {Rook, White});
  board.set_piece(Rank_1, File_H, {Rook, White});
  board.set_piece(Rank_8, File_A, {Rook, Black});
  board.set_piece(Rank_8, File_H, {Rook, Black});

  board.set_piece(Rank_1, File_B, {Knight, White});
  board.set_piece(Rank_1, File_G, {Knight, White});
  board.set_piece(Rank_8, File_B, {Knight, Black});
  board.set_piece(Rank_8, File_G, {Knight, Black});

  board.set_piece(Rank_1, File_C, {Bishop, White});
  board.set_piece(Rank_1, File_F, {Bishop, White});
  board.set_piece(Rank_8, File_C, {Bishop, Black});
  board.set_piece(Rank_8, File_F, {Bishop, Black});

  board.set_piece(Rank_1, File_D, {King, White});
  board.set_piece(Rank_8, File_D, {King, Black});
  board.set_piece(Rank_1, File_E, {Queen, White});
  board.set_piece(Rank_8, File_E, {Queen, Black});
}








