#include <MoveGenerator.h>
#include <cassert>
#include <iostream>
#include <algorithm>

template std::vector<Square>
MoveGenerator::generate_sliding_moves<4>(
    const std::array<MoveDir, 4>&,
    Position
) const;

template std::vector<Square>
MoveGenerator::generate_sliding_moves<8>(
    const std::array<MoveDir, 8>&,
    Position
) const;

std::vector<Square> MoveGenerator::generate_pseudo_legal_moves(Position from_square) {

  switch (from_square.piece.type) {
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
      return generate_king_pseudo_legal_moves(from_square);
    }
    default:
      return {};
  }
}

Piece MoveGenerator::intToPiece(u_int8_t piece) {
  return {
      static_cast<PieceType>(piece & 0x7),
      static_cast<Color>(piece & 0b11000)
    };
}

std::vector<Square> MoveGenerator::generate_pawn_pseudo_legal_moves(Position move) const {
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

  Rank curr_rank = move.curr_square.rank;
  File curr_file = move.curr_square.file;
  Rank starting_rank = move.piece.color == Black ? Rank_7 : Rank_2;
  Color curr_color = move.piece.color;
  Color enemy_color = curr_color == White ? Black : White;
  auto dirs = curr_color == White ? white_directions : black_directions;

  for (const auto&[rd, fd] : dirs) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (new_rank > Rank_8 || new_file > File_H || new_rank < 0 || new_file < 0) {
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
    if (!changes.empty() && canEnPassant(Position{curr_rank, curr_file, move.piece},changes.back())) {
      add(new_rank, new_file);
    }
  }
  return moves;
}



bool MoveGenerator::canEnPassant(const Position current_pos, const MoveChange previous_move) {
  if (previous_move.from.piece.type != Pawn) {
    return false;
  }
  //Check if pawn is currently adjacent to enemy pawn
  if (previous_move.to.curr_square.rank != current_pos.curr_square.rank) {
    return false;
  }
  Color curr_color = current_pos.piece.color;
  int forward = curr_color == White ? 1 : -1;
  Position passantsqr1 {
    static_cast<Rank>(current_pos.curr_square.rank + 1),
    static_cast<File>(current_pos.curr_square.file + forward)
  };
  Position passantsqr2 {
    static_cast<Rank>(current_pos.curr_square.rank - 1),
    static_cast<File>(current_pos.curr_square.file + forward)
  };

  std::array positions{passantsqr1, passantsqr2};
  return std::ranges::any_of(positions, [&] (const Position& p) {
    const int from_rank = previous_move.from.curr_square.rank;
    const int to_rank = previous_move.to.curr_square.rank;
    if (curr_color == White) {
      if (from_rank == p.curr_square.rank + 1 && to_rank == p.curr_square.rank - 1) {
        return true;
      }
    } else if (curr_color == Black) {
      if (from_rank == p.curr_square.rank - 1 && to_rank == p.curr_square.rank + 1) {
        return true;
      }
    }
    return false;
  });
}

std::vector<Square> MoveGenerator::generate_knight_pseudo_legal_moves(Position move) const {
  static std::array knight_dir = {
    MoveDir{1, 2},
    MoveDir{ 1, -2},
    MoveDir{ 2, 1},
    MoveDir{ 2, -1},
    MoveDir{ -1, 2},
    MoveDir{ -1, -2},
    MoveDir{ -2, 1},
    MoveDir{-2, -1}
  };

  std::vector<Square> moves;
  auto add = [&] (int rank, int file) {
    moves.push_back(Square{
      static_cast<Rank>(rank),
      static_cast<File>(file)
    });
  };

  Rank curr_rank = move.curr_square.rank;
  File curr_file = move.curr_square.file;
  Color curr_color = move.piece.color;
  Color enemy_color = curr_color == White ? Black : White;

  for (const auto&[rd, fd] : knight_dir) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (new_rank > Rank_8 || new_file > File_H || new_rank < 0 || new_file < 0) {
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

std::vector<Square> MoveGenerator::generate_bishop_pseudo_legal_moves(Position move) const {
  static std::array bishop_directions = {
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1}
  };
  return generate_sliding_moves(bishop_directions, move);
}

std::vector<Square> MoveGenerator::generate_rook_pseudo_legal_moves(Position move) const {
  static std::array rook_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1}
  };
  return generate_sliding_moves(rook_directions, move);
}

std::vector<Square> MoveGenerator::generate_queen_pseudo_legal_mvoes(Position move) const {
  static std::array queen_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1},
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1}
  };
  return generate_sliding_moves(queen_directions, move);
}

std::vector<Square> MoveGenerator::generate_king_pseudo_legal_moves(Position move, bool kside_castle, bool qside_castle) const {
  static std::array king_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1},
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1},
  };

  std::vector<Square> moves;
  auto add = [&] (int rank, int file) {
    moves.push_back(Square{
      static_cast<Rank>(rank),
      static_cast<File>(file)
    });
  };
  Rank curr_rank = move.curr_square.rank;
  File curr_file = move.curr_square.file;
  Color curr_color = move.piece.color;
  Color enemy_color = curr_color == White ? Black : White;

  for (const auto&[rd, fd] : king_directions) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (new_rank > Rank_8 || new_file > File_H || new_rank < 0 || new_file < 0) {
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
}

template<size_t N>
std::vector<Square> MoveGenerator::generate_sliding_moves(const std::array<MoveDir, N> &dirs, const Position move) const {
  std::vector<Square> moves{};
  Rank curr_rank = move.curr_square.rank;
  File curr_file = move.curr_square.file;
  Color enemy_color = move.piece.color == White ? Black : White;

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

