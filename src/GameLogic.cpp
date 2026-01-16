
#include <GameLogic.h>
#include <util.h>
#include <cassert>
#include <GameTypes.h>

GameLogic::GameLogic() {
  move_history.reserve(1024);
  set_initial_board();
}

Piece GameLogic::intToPiece(u_int8_t piece) {
  return {
      static_cast<PieceType>(piece & 0x7),
      static_cast<Color>(piece & 0b11000)
    };
}

void GameLogic::set_initial_board() {

  for (size_t i = File_A; i <= File_H; i++) {
    board[Rank_2][i] = piece(Color::White, PieceType::Pawn);
    board[Rank_7][i] = piece(Color::Black, PieceType::Pawn);
  }
  
  board[Rank_1][File_A] = piece(Color::White, PieceType::Rook);
  board[Rank_1][File_H] = piece(Color::White, PieceType::Rook);
  board[Rank_8][File_A] = piece(Color::Black, PieceType::Rook);
  board[Rank_8][File_H] = piece(Color::Black, PieceType::Rook);

  board[Rank_1][File_B] = piece(Color::White, PieceType::Knight);
  board[Rank_1][File_G] = piece(Color::White, PieceType::Knight);
  board[Rank_8][File_B] = piece(Color::Black, PieceType::Knight);
  board[Rank_8][File_G] = piece(Color::Black, PieceType::Knight);

  board[Rank_1][File_C] = piece(Color::White, PieceType::Bishop);
  board[Rank_1][File_F] = piece(Color::White, PieceType::Bishop);
  board[Rank_8][File_C] = piece(Color::Black, PieceType::Bishop);
  board[Rank_8][File_F] = piece(Color::Black, PieceType::Bishop);

  board[Rank_1][File_D] = piece(Color::White, PieceType::King);
  board[Rank_8][File_D] = piece(Color::Black, PieceType::King);
  board[Rank_1][File_E] = piece(Color::White, PieceType::Queen);
  board[Rank_8][File_E] = piece(Color::Black, PieceType::Queen);
}



