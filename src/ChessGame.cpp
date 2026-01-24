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

ChessGame::ChessGame()
: move_gen(board)
, piece_list(board.get_piece_list()){
  move_history.reserve(1024);
}

ChessGame::ChessGame(const std::string& fen)
: board(fen)
, move_gen(board)
, piece_list(board.get_piece_list()) {

  std::array<std::string, 6> fields{};
  const std::source_location& location = std::source_location::current();
  std::istringstream fen_fields{fen.data()};
  try {
    for (size_t i{}; i < fields.size(); i++) {
      getline(fen_fields, fields[i], ' ');
    }

    const std::string_view turn_to_move = fields[1];
    const std::string_view castling_rights = fields[2];
    const std::string_view en_passant_square = fields[3];
    const std::string_view halfmove_clock = fields[4];
    const std::string_view fullmove_counter = fields[5];

    set_castling_from_fen(castling_rights);

    if (turn_to_move != "w" && turn_to_move != "b") {
      throw std::runtime_error("Illegal FEN: turn must be 'w' or 'b");
    }
    if (en_passant_square != "-" && en_passant_square.size() == 2) {
      char file = std::tolower(en_passant_square[0]);
      char rank = std::tolower(en_passant_square[1]);
      if (std::isalpha(file) && std::isdigit(rank)) {
        en_passant_target_square = Square{static_cast<Rank>(rank - 1), static_cast<File>(file - 'a')};
        passant_sqr_exists = true;
      }
    } else if (en_passant_square == "-") {
      passant_sqr_exists = false;
    } else {
      throw std::runtime_error("En passant square has wrong format");
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


void ChessGame::apply_move(Move move) {
  const auto[from_r, from_f] = move.from;
  Piece p = board.at(from_r, from_f);
  board.move_piece(p, move.from, move.to);
  if (p.type == King || p.type == Rook) {
    if (move.is_castling()) {
      move_rook(move);
    }
    update_castling_rights(p, move.to);
  }
  if (p.type == Pawn) {
    if (move.is_en_passant && passant_sqr_exists) {
      do_en_passant_capture();
      passant_sqr_exists = false;
    } else if (std::abs(move.to.rank - move.from.rank) == 2) {
      update_en_passant_square(move.to);
    } else if (move.needs_pawn_promotion) {
      promote_piece(move.promote_to, move.to);
    }
  }
  if (current_turn == Black) {
    full_moves += 1;
    current_turn = White;
  } else {
    current_turn = Black;
  }
}

void ChessGame::undo_move() {
  board.undo_last_move();
}

bool ChessGame::is_legal_move(Piece p, Square from, Square to) {
  board.move_piece(p, from, to);
  bool is_chk = is_check();
  board.undo_last_move();
  return is_chk;
}

bool ChessGame::can_promote(Piece p, Square sqr) const {
  if (p.type == Pawn && ((p.color == White && sqr.rank == Rank_8) || (p.color == Black && sqr.rank == Rank_1))) {
    return true;
  }
  return false;
}

std::array<Move, 4> ChessGame::get_promotion_moves(Square from, Square to) {
  std::array<Move, 4> moves{};
  size_t idx{};
  for (auto i : std::array{Rook, Queen, Bishop, Knight}) {
    Move m{from, to};
    m.needs_pawn_promotion = true;
    m.promote_to = i;
    moves.at(i) = m;
    idx++;
  }
  return moves;
}

std::vector<Move> ChessGame::generate_legal_moves(Piece p, Square s) {
  std::vector move_list{move_gen.generate_pseudo_legal_moves(s)};
  std::vector<Move> legal_moves;
  legal_moves.reserve(24);
  for (const Square& sqr : move_list) {
    Move m{s, sqr};
    if (can_promote(p, s)) {
      get_promotion_moves(s, sqr);
    } else {
      legal_moves.push_back(m);
    }
  }
  if (p.type == Pawn && passant_sqr_exists && can_enpassant(p, s)) {
    legal_moves.emplace_back(Move{s, en_passant_target_square});
    legal_moves.back().is_en_passant = true;
  }
  if (p.type == King) {
    legal_moves.append_range(get_castling_squares(s));
  }
  std::erase_if(move_list, [&] (const Square dest) {
    return !is_legal_move(p, s, dest);
  });
  return legal_moves;
}

bool ChessGame::is_check() const {
  Piece king{};
  Square king_sqr{};
  for (const auto& [piece, square] : piece_list) {
    if (piece.type == King && current_turn == piece.color) {
      king = piece;
      king_sqr = square;
      break;
    }
  }
  assert(king.type != NoPiece);
  const auto[curr_rank, curr_file] = king_sqr;
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
    if (!GameBoard::is_inbound(rank, file)) {
      return false;
    }
    Piece p = board.at(static_cast<Rank>(rank), static_cast<File>(file));
    if (p.color == enemy_color && std::ranges::find(check_for, p.type) != check_for.end()) {
      return true;
    }
    return false;
  };

  auto check_for = [&] (std::initializer_list<PieceType> check_for_piece, std::span<const MoveDir> dir, AttackType c) {
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

bool ChessGame::can_enpassant(Piece piece, Square current_pos) const {
  assert(passant_sqr_exists);
  assert(piece.type == Pawn);
  int dir = piece.color == White ? -1 : 1;
  if (piece.color == White && current_pos.rank != Rank_5) {
    return false;
  }
  if (piece.color == Black && current_pos.rank != Rank_4) {
    return false;
  }
  if (en_passant_target_square.rank + dir  != current_pos.rank) {
    return false;
  }
  if (std::abs(en_passant_target_square.file - current_pos.file) != 1) {
    return false;
  }
  return true;
}

const std::vector<std::pair<Piece, Square>>& ChessGame::get_piece_list() {
  return piece_list;
}

/**
 *
 * @param p A piece that is either a King or a Rook of either color
 * @param s The current square that piece is after a move has been made
 */
void ChessGame::update_castling_rights(Piece p, Square s) {
  if (p.type == King) {
    if (p.color == White && !king_moved_w) {
      king_moved_w = true;
    }
    if (p.color == Black && !king_moved_b) {
      king_moved_b = true;
    }
  }
  //Cases where either sides rook has not moved
  if (p.type == Rook && p.color == White) {
    if (!k_rook_white_moved && (s.rank != Rank_1 || s.file != File_H)) {
      k_rook_white_moved = true;
    } else if (!q_rook_white_moved && (s.rank != Rank_1 || s.file != File_A)) {
      q_rook_white_moved = true;
    }
  }
  if (p.type == Rook && p.color == Black) {
    if (!k_rook_black_moved && (s.rank != Rank_8 || s.file != File_H)) {
      k_rook_black_moved = true;
    } else if (!q_rook_black_moved && (s.rank != Rank_8 || s.file != File_A)) {
      q_rook_black_moved = true;
    }
  }
}

void ChessGame::set_castling_from_fen(const std::string_view f) {
  const size_t len = f.length();
  assert(len >= 1 && len <= 4);
  bool can_castle = false;
  for (const auto& c : f) {
    switch (c) {
      case 'K':
        can_castle = true;
      k_rook_white_moved = true;
      break;
      case 'Q':
        can_castle = true;
      q_rook_white_moved = true;
      break;
      case 'k':
        can_castle = true;
      k_rook_black_moved = true;
      break;
      case 'q':
        can_castle = true;
      q_rook_black_moved = true;
      break;
      case '-':
        if (can_castle) {
          throw std::runtime_error("Illegal fen: Cannot include '-' with characters");
        }
      return;
      default:
    }
  }
}

bool ChessGame::check_castle_space(Square king_pos, bool k_side) {
  const auto[k_rank, k_file] = king_pos;
  if (k_side) {
    std::array squares = {
      Square{k_rank, static_cast<File>(k_file + 1)},
      Square{k_rank, static_cast<File>(k_file + 2)}
    };
    return std::ranges::all_of(squares, [&, this] (const Square& sqr) {
      return board.at(sqr.rank, sqr.file).type == NoPiece;
    });
  } else {
    std::array squares = {
      Square{k_rank, static_cast<File>(k_file - 1)},
      Square{k_rank, static_cast<File>(k_file - 2)},
      Square{k_rank, static_cast<File>(k_file - 3)}
    };
    return std::ranges::all_of(squares, [&, this] (const Square& sqr) {
      return board.at(sqr.rank, sqr.file).type == NoPiece;
    });
  }
  return false;
}

std::vector<Move> ChessGame::get_castling_squares(Square king_pos) {
  std::vector<Move> castle_dirs{};
  Piece p = board.at(king_pos.rank, king_pos.file);
  if (p.type != King) {
    return {};
  }
  if (p.color == White) {
    if (king_moved_w || k_rook_white_moved || q_rook_white_moved) {
      return {};
    }
  } else {
    if (king_moved_b || k_rook_black_moved || q_rook_black_moved) {
      return {};
    }
  }

  if (check_castle_space(king_pos, true)) {
    const auto [_, f_dir] = MoveGenerator::k_side_castle_dir;
    Square sqr{
      king_pos.rank,
      static_cast<File>(f_dir + king_pos.file)
    };
    castle_dirs.emplace_back(Move{king_pos, sqr});
    castle_dirs.back().is_k_castle = true;
  }
  if (check_castle_space(king_pos, false)) {
    const auto [r_dir, f_dir] = MoveGenerator::q_side_castle_dir;
    Square sqr{
      king_pos.rank,
      static_cast<File>(f_dir + king_pos.file)
    };
    castle_dirs.emplace_back(Move{king_pos, sqr});
    castle_dirs.back().is_q_castle = true;
  }
  return castle_dirs;
}

void ChessGame::move_rook(const Move &kings_move) {
  auto [king_pos_r, king_pos_f] = kings_move.to;
  Rank side = current_turn == White ? Rank_1 : Rank_8;
  File rook_start = kings_move.is_k_castle ? File_H : File_A;
  File rook_dest = kings_move.is_k_castle ? static_cast<File>(king_pos_f - 1) : static_cast<File>(king_pos_f + 1);

  board.move_piece(
    board.at(side, rook_start),
    Square{side, File_H},
    Square{side, rook_dest}
    );
}

void ChessGame::do_en_passant_capture() {
  assert(passant_sqr_exists);
  int rank_dir = current_turn == White ? -1 : 1;
  Square enemy_squre{static_cast<Rank>(en_passant_target_square.rank + rank_dir), en_passant_target_square.file};
  Piece p = board.at(enemy_squre.rank, enemy_squre.file);
  assert(p.type == Pawn && p.color != current_turn);
  board.remove_piece(enemy_squre);
}

void ChessGame::promote_piece(PieceType promote_to, Square s) {
  std::initializer_list<PieceType> valid_types {Knight, Queen, Bishop, Rook};
  assert(std::ranges::find(valid_types, promote_to) != valid_types.end());
  board.set_piece(Piece{promote_to, current_turn}, s);
}

void ChessGame::update_en_passant_square(Square last_pawn_move) {
  en_passant_target_square = last_pawn_move;
  int dir = current_turn == White ? - 1 : 1;
  en_passant_target_square.rank = static_cast<Rank>(en_passant_target_square.rank + dir);
  passant_sqr_exists = true;
}




