#include <ChessGame.h>
#include <util.h>
#include <cassert>
#include <GameTypes.h>
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
: move_gen(board)
, piece_list(board.get_piece_list()) {

  std::array<std::string, 6> fields{};
  const std::source_location& location = std::source_location::current();
  std::istringstream fen_fields{fen.data()};
  try {
    for (size_t i{}; i < fields.size(); i++) {
      getline(fen_fields, fields[i], ' ');
    }
    board = GameBoard(fields[0]);
    piece_list = board.get_piece_list();
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
        state.en_passant_target_square = Square{static_cast<Rank>(rank - 1), static_cast<File>(file - 'a')};
        state.passant_sqr_exists = true;
      }
    } else if (en_passant_square == "-") {
      state.passant_sqr_exists = false;
    } else {
      throw std::runtime_error("En passant square has wrong format");
    }
    state.current_turn = turn_to_move == "w" ? White : Black;
    state.half_move_clock = std::stoi(halfmove_clock.data());
    state.full_moves = std::stoi(fullmove_counter.data());
  } catch (std::runtime_error& e) {
    throw std::runtime_error(std::format("{}:{}:{}:{}",
      location.file_name(),
      location.function_name(),
      location.line(),
      e.what()));
  }
}


void ChessGame::apply_move(Move move) {
  prev_state.push_back(state);
  const auto[from_r, from_f] = move.from;
  state.passant_sqr_exists = false;
  Piece p = board.at(from_r, from_f);
  board.move_piece(p, move);
  if (p.type == King || p.type == Rook) {
    if (move.is_castling()) {
      Move m  = get_rook_castle_move(move);
      Piece rook = board.at(m.from);
      board.move_piece(rook, m);
      update_castling_rights(rook, m.from);
    }
    update_castling_rights(p, move.from);
  }
  if (p.type == Pawn) {
    if (move.needs_pawn_promotion) {
      promote_piece(move.promote_to, move.to);
    }
    if (std::abs(move.to.rank - move.from.rank) == 2) {
      update_en_passant_square(move.to, p.color);
    }
  }
  if (state.current_turn == Black) {
    state.full_moves += 1;
    state.current_turn = White;
  } else {
    state.current_turn = Black;
  }
}


void ChessGame::undo_move() {
  board.undo_last_move();
  state = prev_state.back();
  prev_state.pop_back();
}


bool ChessGame::is_legal_move(Piece p, Move m) {
  board.move_piece(p, m);
  Square king_sqr{};
  if (p.type != King) {
    for (const auto& [piece, square] : piece_list) {
      if (piece.type == King && state.current_turn == piece.color) {
        king_sqr = square;
        break;
      }
    }
  } else {
    king_sqr = m.to;
    if (m.is_castling()) {
      Move mv = get_rook_castle_move(m);
      board.move_piece(board.at(mv.from), mv);
      board.undo_last_move();
    }
  }
  const bool illegal_move = is_check(king_sqr);
  board.undo_last_move();
  return !illegal_move;
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
    moves.at(idx) = m;
    idx++;
  }
  return moves;
}


std::vector<Move> ChessGame::generate_legal_moves(Piece p, Square source) {
  std::vector move_list{move_gen.generate_pseudo_legal_moves(source)};
  std::vector<Move> legal_moves;
  legal_moves.reserve(24);
  for (const Square& dest_sqr : move_list) {
    Move m{source, dest_sqr};
    if (can_promote(p, dest_sqr)) {
      auto prom = get_promotion_moves(source, dest_sqr);
      legal_moves.insert(legal_moves.end(), prom.begin(), prom.end());
    } else {
      legal_moves.push_back(m);
    }
  }
  if (p.type == Pawn && state.passant_sqr_exists && can_enpassant(p, source)) {
    Move m {source, state.en_passant_target_square};
    m.is_en_passant = true;
    legal_moves.push_back(m);
  }
  if (p.type == King) {
    legal_moves.append_range(get_castling_squares(source));
  }
  std::erase_if(legal_moves, [&] (const Move& move) {
    return !is_legal_move(p, move);
  });
  return legal_moves;
}


bool ChessGame::is_check(Square s) const {
  const auto[curr_rank, curr_file] = s;
  Color enemy_color = state.current_turn == White ? Black : White;
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
  if (check_for({Queen, Rook}, MoveGenerator::rook_directions, sliding_attacks)) {
    return true;
  }
  if (check_for({Queen, Bishop}, MoveGenerator::bishop_directions, sliding_attacks)) {
    return true;
  }
  return false;
}


bool ChessGame::can_enpassant(Piece piece, Square current_pos) const {
  assert(state.passant_sqr_exists == true);
  assert(piece.type == Pawn);
  int dir = piece.color == White ? 1 : -1;
  if (piece.color == White && current_pos.rank != Rank_5) {
    return false;
  }
  if (piece.color == Black && current_pos.rank != Rank_4) {
    return false;
  }
  if (state.en_passant_target_square.rank != current_pos.rank + dir) {
    return false;
  }
  if (std::abs(state.en_passant_target_square.file - current_pos.file) != 1) {
    return false;
  }
  return true;
}


/**
 *
 * @param p A piece that is either a King or a Rook of either color
 * @param s The current square that piece is after a move has been made
 */
void ChessGame::update_castling_rights(Piece p, Square src) {
  if (p.type == King) {
    if (p.color == White && !state.king_moved_w) {
      state.king_moved_w = true;
    }
    if (p.color == Black && !state.king_moved_b) {
      state.king_moved_b = true;
    }
  }
  //Cases where either sides rook has not moved
  if (p.type == Rook && p.color == White) {
    if (src.file == File_H) {
      state.k_rook_white_moved = true;
    } else if (src.file == File_A) {
      state.q_rook_white_moved = true;
    }
  }
  if (p.type == Rook && p.color == Black) {
    if (src.file == File_H) {
      state.k_rook_black_moved = true;
    } else if (src.file == File_A) {
      state.q_rook_black_moved = true;
    }
  }
}


bool ChessGame::can_k_side_castle(Square king_pos) {
  Piece p = board.at(king_pos.rank, king_pos.file);
  if (p.type != King) {
    return false;
  }
  if (is_check(king_pos)) {
    return false;
  }
  if (p.color == White) {
    if (state.king_moved_w || state.k_rook_white_moved) {
      return false;
    }
  } else {
    if (state.king_moved_b || state.k_rook_black_moved) {
      return false;
    }
  }
  const auto [r, f] = king_pos;
  Square sqr1{r, static_cast<File>(f + 1)};
  Square sqr2{r, static_cast<File>(f + 2)};
  if (board.at(sqr1).type != NoPiece) return false;
  if (board.at(sqr2).type != NoPiece) return false;
  if (is_check(sqr1)) return false;
  if (is_check(sqr2)) return false;
  return true;
}


bool ChessGame::can_q_side_castle(Square king_pos) {
  Piece p = board.at(king_pos.rank, king_pos.file);
  if (p.type != King) return false;
  if (is_check(king_pos)) return false;
  if (p.color == White) {
    if (state.king_moved_w || state.q_rook_white_moved) return false;
  } else {
    if (state.king_moved_b || state.q_rook_black_moved) return false;
  }
  const auto [r, f] = king_pos;
  Square d{r, static_cast<File>(f - 1)};
  Square c{r, static_cast<File>(f - 2)};
  Square b{r, static_cast<File>(f - 3)};
  if (board.at(d).type != NoPiece) return false;
  if (board.at(c).type != NoPiece) return false;
  if (board.at(b).type != NoPiece) return false;
  if (is_check(d)) return false;
  if (is_check(c)) return false;
  return true;
}


std::vector<Move> ChessGame::get_castling_squares(Square king_pos) {
  std::vector<Move> castle_dirs{};
  Piece p = board.at(king_pos.rank, king_pos.file);
  assert(p.type == King);

  if (can_k_side_castle(king_pos)) {
    const auto [_, f_dir] = MoveGenerator::k_side_castle_dir;
    Square sqr{
      king_pos.rank,
      static_cast<File>(f_dir + king_pos.file)
    };
    castle_dirs.emplace_back(Move{king_pos, sqr});
    castle_dirs.back().is_k_castle = true;
  }
  if (can_q_side_castle(king_pos)) {
    const auto [_, f_dir] = MoveGenerator::q_side_castle_dir;
    Square sqr{
      king_pos.rank,
      static_cast<File>(f_dir + king_pos.file)
    };
    castle_dirs.emplace_back(Move{king_pos, sqr});
    castle_dirs.back().is_q_castle = true;
  }
  return castle_dirs;
}


Move ChessGame::get_rook_castle_move(const Move &kings_move) {
  auto [king_pos_r, king_pos_f] = kings_move.to;
  const Piece king = board.at(kings_move.from);
  assert(king.type == King);
  const Rank side = king.color == White ? Rank_1 : Rank_8;
  const File rook_start = kings_move.is_k_castle ? File_H : File_A;
  const File rook_dest = kings_move.is_k_castle ? static_cast<File>(king_pos_f - 1) : static_cast<File>(king_pos_f + 1);
  const Move m{Square{side, rook_start}, Square{side, rook_dest} };
  return m;
}


void ChessGame::promote_piece(PieceType promote_to, Square s) {
  std::initializer_list<PieceType> valid_types {Knight, Queen, Bishop, Rook};
  assert(std::ranges::find(valid_types, promote_to) != valid_types.end());
  board.set_piece(Piece{promote_to, state.current_turn}, s);
}


void ChessGame::update_en_passant_square(Square last_pawn_move, Color pawn_color) {
  state.en_passant_target_square = last_pawn_move;
  int dir = pawn_color == White ? - 1 : 1;
  state.en_passant_target_square.rank = static_cast<Rank>(state.en_passant_target_square.rank + dir);
  state.passant_sqr_exists = true;
}


void ChessGame::set_castling_from_fen(const std::string_view f) {
  const size_t len = f.length();
  assert(f == "-" || (len >= 1 && len <= 4 && f != "-"));
  bool can_castle = false;
  for (const auto& c : f) {
    switch (c) {
      case 'K':
        can_castle = true;
      state.k_rook_white_moved = false;
      break;
      case 'Q':
        can_castle = true;
      state.q_rook_white_moved = false;
      break;
      case 'k':
        can_castle = true;
      state.k_rook_black_moved = false;
      break;
      case 'q':
        can_castle = true;
      state.q_rook_black_moved = false;
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


Color ChessGame::get_current_turn() const {
  return state.current_turn;
}


const std::vector<std::pair<Piece, Square>>& ChessGame::get_piece_list() {
  return piece_list;
}



