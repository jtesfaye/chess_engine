#include <MoveGenerator.h>
#include <cassert>
#include <iostream>
#include <algorithm>

template std::vector<Square>
MoveGenerator::generate_sliding_moves<4>(
    const std::array<MoveDir, 4>&,
    Piece
) const;

template std::vector<Square>
MoveGenerator::generate_sliding_moves<8>(
    const std::array<MoveDir, 8>&,
    Piece
) const;


std::vector<Square> MoveGenerator::generate_pseudo_legal_moves(Piece from_square, bool can_kcastle, bool can_qcastle) {
  switch (from_square.type) {
    case Pawn: {
        return generate_pawn_pseudo_legal_moves(from_square);
      }
    case Knight: {
      return generate_knight_pseudo_legal_moves(from_square);
    }
    case Bishop: {
      return generate_bishop_pseudo_legal_moves(from_square);
    }
    case Rook: {
      return generate_rook_pseudo_legal_moves(from_square);
    }
    case Queen: {
      return generate_queen_pseudo_legal_mvoes(from_square);
    }
    case King: {
      return generate_king_pseudo_legal_moves(from_square, can_kcastle, can_qcastle);
    }
    default:
      return {};
  }
}

bool MoveGenerator::bound_check(const int r, const int f) {
  return r > Rank_8 || f > File_H || r < 0 || f < 0;
}


Piece MoveGenerator::intToPiece(u_int8_t piece) {
  return {
      static_cast<PieceType>(piece & 0x7),
      static_cast<Color>(piece & 0b11000)
    };
}

std::vector<Square> MoveGenerator::generate_pawn_pseudo_legal_moves(Piece move) const {
  static std::array white_directions = {
    MoveDir{ 1, 0},
    MoveDir{ 2, 0},
    MoveDir{ 1, 1},
    MoveDir{ 1, -1}
  };

  static std::array black_directions = {
    MoveDir{ -1, 0},
    MoveDir{ -2, 0},
    MoveDir{ -1, -1},
    MoveDir{ -1, 1}
  };

  std::vector<Square> moves;
  auto add = [&] (int r, int f) {
    moves.push_back(Square{
      static_cast<Rank>(r),
      static_cast<File>(f)
    });
  };

  auto is_one_forward = [&] (int rd, int fd) {
    if ((rd == 1 && fd == 0) || (rd == -1 && fd == 0)) {
      return true;
    }
    return false;
  };

  Rank curr_rank = move.position.rank;
  File curr_file = move.position.file;
  Rank starting_rank = move.color == Black ? Rank_7 : Rank_2;
  Color curr_color = move.color;
  Color enemy_color = curr_color == White ? Black : White;
  auto dirs = curr_color == White ? white_directions : black_directions;

  for (const auto&[rd, fd] : dirs) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (bound_check(new_rank, new_file)) {
      continue;
    }
    Piece new_space = board.at(static_cast<Rank>(new_rank), static_cast<File>(new_file));
    if (is_one_forward(rd, fd) && new_space.type == NoPiece) {
      add(new_rank, new_file);
      continue;
    }
    if (curr_rank == starting_rank && (rd == 2 || rd == -2) && new_space.type == NoPiece) {
      add(new_rank, new_file);
      continue;
    }
    if (new_file != curr_file && new_space.color == enemy_color) {
      add(new_rank, new_file);
      continue;
    }

    if (!changes.empty() && canEnPassant(move,changes.back())) {
      add(new_rank, new_file);
    }
  }
  return moves;
}

bool MoveGenerator::canEnPassant(const Piece current_pos, const MoveChange previous_move) {
  if (previous_move.from.type != Pawn) {
    return false;
  }
  //Check if pawn is currently adjacent to enemy pawn
  if (previous_move.to.position.rank != current_pos.position.rank) {
    return false;
  }
  Color curr_color = current_pos.color;
  int forward = curr_color == White ? 1 : -1;
  Piece passantsqr1 {
    NoPiece,
    NoColor,
    static_cast<Rank>(current_pos.position.rank + 1),
    static_cast<File>(current_pos.position.file + forward)
  };
  Piece passantsqr2 {
    NoPiece,
    NoColor,
    static_cast<Rank>(current_pos.position.rank - 1),
    static_cast<File>(current_pos.position.file + forward)
  };

  std::array positions{passantsqr1, passantsqr2};
  return std::ranges::any_of(positions, [&] (const Piece& p) {
    const int from_rank = previous_move.from.position.rank;
    const int to_rank = previous_move.to.position.rank;
    if (curr_color == White) {
      if (from_rank == p.position.rank + 1 && to_rank == p.position.rank - 1) {
        return true;
      }
    } else if (curr_color == Black) {
      if (from_rank == p.position.rank - 1 && to_rank == p.position.rank + 1) {
        return true;
      }
    }
    return false;
  });
}

std::vector<Square> MoveGenerator::generate_knight_pseudo_legal_moves(Piece move) const {

  std::vector<Square> moves;
  auto add = [&] (int rank, int file) {
    moves.push_back(Square{
      static_cast<Rank>(rank),
      static_cast<File>(file)
    });
  };

  Rank curr_rank = move.position.rank;
  File curr_file = move.position.file;
  Color curr_color = move.color;
  Color enemy_color = curr_color == White ? Black : White;

  for (const auto&[rd, fd] : knight_dir) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (bound_check(new_rank, new_file)) {
      continue;
    }
    Piece new_space = board.at(static_cast<Rank>(new_rank), static_cast<File>(new_file));
    if (new_space.color == curr_color) {
      continue;
    }
    if (new_space.type == NoPiece || new_space.color == enemy_color) {
      add(new_rank, new_file);
    }
  }
  return moves;
}

std::vector<Square> MoveGenerator::generate_bishop_pseudo_legal_moves(Piece move) const {

  return generate_sliding_moves(bishop_directions, move);
}

std::vector<Square> MoveGenerator::generate_rook_pseudo_legal_moves(Piece move) const {

  return generate_sliding_moves(rook_directions, move);
}

std::vector<Square> MoveGenerator::generate_queen_pseudo_legal_mvoes(Piece move) const {

  return generate_sliding_moves(queen_directions, move);
}

std::vector<Square> MoveGenerator::generate_king_pseudo_legal_moves(Piece move, bool can_kcastle, bool can_qcastle) const {

  std::vector<Square> moves;
  auto add = [&] (int rank, int file) {
    moves.push_back(Square{
      static_cast<Rank>(rank),
      static_cast<File>(file)
    });
  };
  Rank curr_rank = move.position.rank;
  File curr_file = move.position.file;
  Color curr_color = move.color;
  Color enemy_color = curr_color == White ? Black : White;

  for (const auto&[rd, fd] : king_directions) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (bound_check(new_rank, new_file)) {
      continue;
    }
    Piece new_space = board.at(static_cast<Rank>(new_rank), static_cast<File>(new_file));
    if (new_space.color == curr_color) {
      continue;
    }
    if (new_space.type == NoPiece || new_space.color == enemy_color) {
      add(new_rank, new_file);
    }
  }
  if (can_kcastle) {
    int new_file = static_cast<File>(curr_file + 2);
    add(curr_rank, new_file);
  }
  if (can_qcastle) {
    int new_file = static_cast<File>(curr_file - 2);
    add(curr_rank, new_file);
  }
  return moves;
}

template<size_t N>
std::vector<Square> MoveGenerator::generate_sliding_moves(const std::array<MoveDir, N> &dirs, const Piece move) const {
  std::vector<Square> moves{};
  Rank curr_rank = move.position.rank;
  File curr_file = move.position.file;
  Color enemy_color = move.color == White ? Black : White;

  for (const auto& [rank_dir, file_dir] : dirs) {
    int new_rank  = curr_rank + rank_dir;
    int new_file = curr_file + file_dir;
    while (new_rank >= Rank_1 && new_rank <= Rank_8 && new_file >= File_A && new_file <= File_H) {
      Piece p = board.at(static_cast<Rank>(new_rank), static_cast<File>(new_file));
      if (p.type == NoPiece) {
        moves.push_back({static_cast<Rank>(new_rank), static_cast<File>(new_file)});
      } else {
        if (p.color == enemy_color) {
          moves.push_back({static_cast<Rank>(new_rank), static_cast<File>(new_file)});
        }
        break;
      }
      new_rank += rank_dir;
      new_file += file_dir;
    }
  }
  return moves;
}

