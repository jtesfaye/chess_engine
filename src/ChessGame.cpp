#include <ChessGame.h>
#include <util.h>
#include <cassert>
#include <GameTypes.h>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include <span>
#include <sstream>
#include <format>
#include<source_location>

ChessGame::ChessGame() : move_gen(board, move_history) {
  move_history.reserve(1024);
  set_initial_board();
}

ChessGame::ChessGame(const std::string& fen)
: move_gen(board, move_history) {

  std::array<std::string, 6> fields;
  const std::source_location& location = std::source_location::current();
  const std::string_view piece_placement = fields[0];
  const std::string_view turn_to_move = fields[1];
  const std::string_view castling_rights = fields[2];
  const std::string_view halfmove_clock = fields[4];
  const std::string_view fullmove_counter = fields[5];

  std::istringstream fen_fields{fen.data()};
  try {
    for (size_t i{}; i < fields.size(); i++) {
      if (!getline(fen_fields, fields[i])) {
        throw std::runtime_error("Illegal FEN: less than 6 fields");
      };
    }
    piece_list = std::move(board.load_from_fen_piece_placement(piece_placement.data()));
    set_castling_from_fen(castling_rights);
    if (turn_to_move.at(0) != 'w' || turn_to_move.at(0) != 'b') {
      throw std::runtime_error("Illegal FEN: turn must be 'w' or 'b");
    }
    current_turn = turn_to_move == "w" ? White : Black;
    half_move_clock = std::stoi(halfmove_clock.data());
    full_moves = std::stoi(fullmove_counter.data());
  } catch (std::runtime_error& e) {
    throw std::runtime_error(std::format("{}:{}:{}:{}",
      location.file_name(),
      location.function_name(),
      location.line(),
      e.what()));
  }
}

void ChessGame::set_castling_from_fen(std::string_view f) {
  size_t len = f.size();
  assert(len >= 1 && len <= 4);
  bool can_castle = false;
  for (const auto& c : f) {
    switch (c) {
      case 'K':
        can_castle = true;
        k_rook_white = true;
      case 'Q':
        can_castle = true;
        q_rook_white = true;
      case 'k':
        can_castle = true;
        k_rook_black = true;
      case 'q':
        can_castle = true;
        q_rook_black = true;
      case '-':
        if (can_castle) {
          throw std::runtime_error("Illegal fen: Cannot include '-' with characters");
        }
        return;
      default:
    }
  }
}

void ChessGame::set_initial_board() {
  static std::vector pieces = {
    Piece {Rook, White, Rank_1, File_A},
    Piece {Rook, White, Rank_1, File_H},
    Piece {Rook, Black, Rank_8, File_A},
    Piece {Rook, Black, Rank_8, File_H},
    Piece {Knight, White, Rank_1, File_B},
    Piece {Knight, White, Rank_1, File_G},
    Piece {Knight, Black, Rank_8, File_B},
    Piece {Knight, Black, Rank_8, File_G},
    Piece {Bishop, White, Rank_1, File_C},
    Piece {Bishop, White, Rank_1, File_F},
    Piece {Bishop, White, Rank_8, File_C},
    Piece {Bishop, White, Rank_8, File_F},
    Piece {King, White, Rank_1, File_D},
    Piece {King, Black, Rank_8, File_D},
    Piece {Queen, Black, Rank_8, File_E},
    Piece {Queen, White, Rank_1, File_E}
  };
  for (size_t i = File_A; i <= File_H; i++) {
    Piece white_pawn{Pawn, White, Rank_2, static_cast<File>(i)};
    Piece black_pawn{Pawn, Black, Rank_7, static_cast<File>(i)};
    pieces.push_back(white_pawn);
    pieces.push_back(black_pawn);
  }
  for (const auto& p : pieces) {
    piece_list.push_back(p);
    board.set_piece(p);
  }
}

bool ChessGame::is_check(const GameBoard &board, const std::vector<Piece>& piece_list, Color curr_turn) const {
  Piece king{NoPiece, NoColor};
  for (const auto& p : piece_list) {
    if (p.type == King && curr_turn == p.color) {
      king = p;
      break;
    }
  }
  assert(king.type != NoPiece);
  int curr_rank = king.position.rank;
  int curr_file = king.position.file;
  Color enemy_color = king.color == White ? Black : White;
  using AttackType = std::function<bool(std::initializer_list<PieceType> check_for, int curr_rank, int curr_file, int rank_dir, int file_dir)>;

  AttackType sliding_attacks = [&] (std::initializer_list<PieceType> check_for, int rank, int file, int rank_dir, int file_dir) {
    while (rank >= Rank_1 && rank <= Rank_8 && file >= File_A && file <= File_H) {
      Piece p = board.at(static_cast<Rank>(rank), static_cast<File>(file));
      if (p.color == enemy_color && std::ranges::find(check_for, p.type) != check_for.end()) {
        return true;
      }
      if (p.color != NoColor) {
        break;
      }
      rank += rank_dir;
      file += file_dir;
    }
    return false;
  };

  AttackType regular_attacks = [&] (std::initializer_list<PieceType> check_for, int rank, int file, int rank_dir, int file_dir) {
    if (MoveGenerator::bound_check(rank, file)) {
      return false;
    }
    Piece p = board.at(static_cast<Rank>(rank), static_cast<File>(file));
    if (p.color == enemy_color && std::ranges::find(check_for, p.type) != check_for.end()) {
      return true;
    }
    return false;
  };

  auto check_for = [&] (std::initializer_list<PieceType> check_for_piece, std::span<MoveDir> dir, AttackType c) {
    return std::ranges::any_of(dir, [&] (const MoveDir& d) {
      int new_rank = curr_rank + d.to_rank;
      int new_file = curr_file + d.to_file;
      return c(check_for_piece, new_rank, new_file, d.to_rank, d.to_file);
    });
  };

  int pawn_attack_dir = enemy_color == Black ? 1 : -1;
  auto pawn_dirs = std::array{MoveDir{pawn_attack_dir, 1}, MoveDir{pawn_attack_dir, -1}};
  if (check_for({Pawn}, pawn_dirs, regular_attacks)) {
    return true;
  }
  if (check_for({Knight}, MoveGenerator::knight_dir, regular_attacks)) {
    return true;
  }
  if (check_for({King}, MoveGenerator::king_directions, regular_attacks)) {
    return true;
  }
  if (check_for({Queen, Bishop, Rook}, MoveGenerator::queen_directions, sliding_attacks)) {
    return true;
  }
  return false;
}







