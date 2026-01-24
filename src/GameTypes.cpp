//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <GameTypes.h>
#include <cassert>
#include <iostream>
#include <utility>

GameBoard::GameBoard() {
  set_initial_board();
}

GameBoard::GameBoard(std::string fen_str) :
piece_list(load_from_fen_piece_placement(std::move(fen_str))){}

void GameBoard::set_initial_board() {
  static std::vector<std::pair<Piece, Square>> pieces =
  {
    {Piece {Rook, White}, Square{Rank_1, File_A}},
    {Piece {Rook, White}, Square{Rank_1, File_H}},
    {Piece {Rook, Black}, Square{Rank_8, File_A}},
    {Piece {Rook, Black}, Square{Rank_8, File_H}},
    {Piece {Knight, White}, Square {Rank_1, File_B}},
    {Piece {Knight, White}, Square {Rank_1, File_G}},
    {Piece {Knight, Black}, Square {Rank_8, File_B}},
    {Piece {Knight, Black}, Square {Rank_8, File_G}},
    {Piece {Bishop, White}, Square {Rank_1, File_C}},
    {Piece {Bishop, White}, Square {Rank_1, File_F}},
    {Piece {Bishop, Black}, Square {Rank_8, File_C}},
    {Piece {Bishop, Black}, Square {Rank_8, File_F}},
    {Piece {King, White}, Square {Rank_1, File_D}},
    {Piece {King, Black}, Square {Rank_8, File_D}},
    {Piece {Queen, Black},Square {Rank_8, File_E}},
    {Piece {Queen, White}, Square {Rank_1, File_E}}
  };

  for (size_t i = File_A; i <= File_H; i++) {
    Piece white_pawn{Pawn, White};
    Piece black_pawn{Pawn, Black};
    pieces.push_back({white_pawn, {Rank_2, static_cast<File>(i)}});
    pieces.push_back({black_pawn, {Rank_7, static_cast<File>(i)}});
  }
  for (const auto& p : pieces) {
    piece_list.push_back(p);
    set_piece(p.first, p.second);
  }
}

Piece GameBoard::at(const Rank r, const File f) const {
  u_int8_t piece = board[r][f];
  return {
    static_cast<PieceType>(piece & 0x7),
    static_cast<Color>(piece & 0b11000)
  };
};

Piece GameBoard::at(Square s) const {
  u_int8_t piece = board[s.rank][s.file];
  return {
    static_cast<PieceType>(piece & 0x7),
    static_cast<Color>(piece & 0b11000)
  };
}

bool GameBoard::is_capture(const Piece p, Square to_square) {
  Color enemy_color = p.color == White ? Black : White;
  if (this->at(to_square).color == enemy_color) {
    return true;
  }
  return false;
}

bool GameBoard::undo_last_move() {
  if (move_history.empty()) {
    return false;
  }
  auto [move, was_captured] = move_history.back();
  Piece p = this->at(move.to);
  set_piece(p, move.from);
  board[move.to.rank][move.to.file] = 0;
  set_piece(was_captured, move.to);
  move_history.pop_back();
  return true;
}

bool GameBoard::is_inbound(const int r, const int f) {
  return !(r > Rank_8 || f > File_H || r < 0 || f < 0);
}

bool GameBoard::remove_piece(Square s) {
  auto [r, f] = s;
  if (!is_inbound(r,f)) {
    return false;
  }
  board[s.rank][s.file] = 0;
  return std::erase_if(piece_list, [&](Position& var) {
      auto [target_r, target_f] = var.second;
      return r == target_r && f == target_f;
  });
}

bool GameBoard::move_piece(const Piece p, const Square from, const Square to) {
  const auto [dest_r, dest_f] = to;
  const auto [from_r, from_f] = to;
  if (!is_inbound(dest_r, dest_f) || !is_inbound(from_r, from_f)) {
    return false;
  }
  if (board[from_r][from_f] == 0) {
    return false;
  }
  Move m{from, to};
  Piece captured{NoPiece, NoColor};
  move_history.emplace_back(MoveChange{m});
  if (is_capture(p, to)) {
    //Update piece list
    auto piece = std::find_if(piece_list.begin(), piece_list.end(), [&](Position& var) {
      auto [target_r, target_f] = var.second;
      return dest_r == target_r && dest_f == target_f;
    });
    captured = piece->first;
    remove_piece(to);
  }
  board[from_r][from_f] = 0;
  board[dest_r][dest_f] = piece(p.color, p.type);
  move_history.back().was_captured = captured;
  return true;
}

bool GameBoard::set_piece(Piece p, Square s) {
  if (!is_inbound(s.rank, s.file)) {
    return false;
  }
  board[s.rank][s.file] = piece(p.color, p.type);
  return true;
}

std::vector<std::pair<Piece, Square>> GameBoard::load_from_fen_piece_placement(std::string fen) {
  std::memset(board.data(), 0, sizeof(board));
  std::stringstream b{fen};
  std::vector<std::string> ranks;
  std::vector<std::pair<Piece, Square>> piece_list;
  std::string r;
  while (std::getline(b, r, '/')) {
    ranks.push_back(std::move(r));
  }
  std::reverse(ranks.begin(), ranks.end());
  for (int i{}; i < ranks.size(); i++) {
    std::string_view rank = ranks[i];
    size_t file{};
    for (const auto& c : rank) {
      if (isdigit(c)) {
        file += c - '0';
      } else {
        piece_list.emplace_back(
          std::make_pair<Piece, Square>(char_to_piece(c), {static_cast<Rank>(i), static_cast<File>(file)})
          );
        set_piece(piece_list.back().first, piece_list.back().second);
        ++file;
      }
    }
  }
  return piece_list;
}

Piece GameBoard::intToPiece(u_int8_t pos) {
  return {
    static_cast<PieceType>(pos & 0x7),
    static_cast<Color>(pos & 0b11000)
  };
}

std::string GameBoard::to_fen_piece_placement() const {
  std::ostringstream out;
  for (int rank = Rank_8; rank >= Rank_1; --rank) {
    int empty = 0;
    for (size_t file = File_A; file <= File_H; ++file) {
      uint8_t p = board[rank][file];
      if (p == 0) {
        ++empty;
      } else {
        if (empty > 0) {
          out << empty;
          empty = 0;
        }
        Piece piece = intToPiece(p);
        char c;
        switch (piece.type) {
          case Pawn:   c = 'p'; break;
          case Knight: c = 'n'; break;
          case Bishop: c = 'b'; break;
          case Rook:   c = 'r'; break;
          case Queen:  c = 'q'; break;
          case King:   c = 'k'; break;
          default:     c = '?'; break;
        }
        if (piece.color == White) {
          c = std::toupper(c);
        }
        out << c;
      }
    }
    if (empty > 0) {
      out << empty;
    }
    if (rank != Rank_1) {
      out << '/';
    }
  }
  return out.str();
}

Piece GameBoard::char_to_piece(char c) {
  switch (c) {
    case 'r':
      return {Rook, Black};
    case 'n':
      return {Knight, Black};
    case 'b':
      return {Bishop, Black};
    case 'k':
      return {King, Black};
    case 'q':
      return {Queen, Black};
    case 'p':
      return {Pawn, Black};
    case 'R':
      return {Rook, White};
    case 'N':
      return {Knight, White};
    case 'B':
      return {Bishop, White};
    case 'K':
      return {King, White};
    case 'Q':
      return {Queen, White};
    case 'P':
      return {Pawn, White};
    default:
      return {NoPiece, NoColor};
  }
}

std::vector<Position> &GameBoard::get_piece_list() {
  return piece_list;
}


