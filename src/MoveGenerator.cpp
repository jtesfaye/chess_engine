#include <MoveGenerator.h>
#include <cassert>
#include <iostream>
#include <algorithm>

std::vector<Square> MoveGenerator::generate_pseudo_legal_moves(Square from_square) {
  Piece piece = board.at(from_square.rank, from_square. file);
  switch (piece.type) {
    case Pawn: {
        return generate_pawn_pseudo_legal_moves(piece, from_square);
      }
    case Knight: {
      return generate_knight_pseudo_legal_moves(piece, from_square);
    }
    case Bishop: {
      return generate_bishop_pseudo_legal_moves(piece, from_square);
    }
    case Rook: {
      return generate_rook_pseudo_legal_moves(piece, from_square);
    }
    case Queen: {
      return generate_queen_pseudo_legal_moves(piece, from_square);
    }
    case King: {
      return generate_king_pseudo_legal_moves(piece ,from_square);
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

std::vector<Square> MoveGenerator::generate_pawn_pseudo_legal_moves(Piece p, Square s) const {
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

  auto [curr_rank, curr_file] = s;
  Rank starting_rank = p.color == Black ? Rank_7 : Rank_2;
  Color curr_color = p.color;
  Color enemy_color = curr_color == White ? Black : White;

  auto dirs = curr_color == White ? white_directions : black_directions;
  for (const auto&[rd, fd] : dirs) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (!GameBoard::is_inbound(new_rank, new_file)) {
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
    }
  }
  return moves;
}

std::vector<Square> MoveGenerator::generate_knight_pseudo_legal_moves(Piece p, Square s) const {
  std::vector<Square> moves;
  auto add = [&] (int rank, int file) {
    moves.push_back(Square{
      static_cast<Rank>(rank),
      static_cast<File>(file)
    });
  };
  const auto [curr_rank, curr_file] = s;
  const Color curr_color = p.color;
  const Color enemy_color = curr_color == White ? Black : White;
  for (const auto&[rd, fd] : knight_dir) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (!GameBoard::is_inbound(new_rank, new_file)) {
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

std::vector<Square> MoveGenerator::generate_bishop_pseudo_legal_moves(Piece p, Square s) const {
  return generate_sliding_moves(bishop_directions, p, s);
}

std::vector<Square> MoveGenerator::generate_rook_pseudo_legal_moves(Piece p, Square s) const {
  return generate_sliding_moves(rook_directions, p, s);
}

std::vector<Square> MoveGenerator::generate_queen_pseudo_legal_moves(Piece p, Square s) const {
  return generate_sliding_moves(queen_directions, p, s);
}

std::vector<Square> MoveGenerator::generate_king_pseudo_legal_moves(Piece p, Square s) const {
  std::vector<Square> moves;
  auto add = [&] (int rank, int file) {
    moves.push_back(Square{
      static_cast<Rank>(rank),
      static_cast<File>(file)
    });
  };
  const auto[curr_rank, curr_file] = s;
  Color curr_color = p.color;
  Color enemy_color = curr_color == White ? Black : White;
  for (const auto&[rd, fd] : king_directions) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    if (!GameBoard::is_inbound(new_rank, new_file)) {
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

template std::vector<Square>
MoveGenerator::generate_sliding_moves<4>(
    const std::array<MoveDir, 4>&,
    Piece,
    Square
) const;

template std::vector<Square>
MoveGenerator::generate_sliding_moves<8>(
    const std::array<MoveDir, 8>&,
    Piece,
    Square
) const;

template<size_t N>
std::vector<Square> MoveGenerator::generate_sliding_moves
(
  const std::array<MoveDir, N> &dirs,
  const Piece move,
  const Square s) const
{
  std::vector<Square> moves{};
  const auto[curr_rank, curr_file] = s;
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

