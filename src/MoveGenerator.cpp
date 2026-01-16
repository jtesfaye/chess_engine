#include <MoveGenerator.h>
#include <cassert>
#include <iostream>
#include <algorithm>

std::vector<Square> MoveGenerator::generate_pseudo_legal_moves(Position from_square) {

  switch (from_square.piece.type) {
    case Pawn: {
        return generate_pawn_pseudo_legal_moves(from_square);
      }
    case Knight: {
      return generate_knight_pseudo_legal_moves(from_square);
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
  static std::array<MoveDir, 4> white_directions = {
    MoveDir{MoveType::Regular, 1, 0},
    MoveDir{MoveType::Regular, 2, 0},
    MoveDir{MoveType::Capture, 1, 1},
    MoveDir{MoveType::Capture, 1, -1}
  };

  static std::array<MoveDir, 4> black_directions = {
    MoveDir{MoveType::Regular, -1, 0},
    MoveDir{MoveType::Regular, -2, 0},
    MoveDir{MoveType::Capture, -1, -1},
    MoveDir{MoveType::Capture, -1, 1}
  };

  std::vector<Square> moves;
  auto add = [&] (int r, int f) {
    moves.push_back(Square{
      static_cast<Rank>(r),
      static_cast<File>(f)
    });
  };

  Rank curr_rank = move.curr_square.rank;
  File curr_file = move.curr_square.file;
  Rank starting_rank = move.piece.color == Black ? Rank_7 : Rank_2;
  Color curr_color = move.piece.color;
  Color enemy_color = curr_color == White ? Black : White;
  auto dirs = curr_color == White ? white_directions : black_directions;

  for (const auto&[type, rd, fd] : dirs) {
    int new_rank = curr_rank + rd;
    int new_file = curr_file + fd;
    Piece newSpace = intToPiece(board[new_rank][new_file]);

    if (type == Regular) {
      if (newSpace.type != NoPiece || new_rank > Rank_8) {
        continue;
      }
      if (curr_rank == starting_rank && (rd == 2 || rd == -2)) {
        add(new_rank, new_file);
        continue;
      }
      add(new_rank, new_file);
    }
    if (type == Capture) {
      if (newSpace.type == NoPiece) {
        continue;
      }
      if (newSpace.color == enemy_color) {
        add(new_rank, new_file);
        continue;
      }
      if (!changes.empty() && canEnPassant(Position{curr_rank, curr_file, move.piece},
          changes.back())) {
        add(new_rank, new_file);
      }
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

std::vector<Square> MoveGenerator::generate_knight_pseudo_legal_moves(Position move) {
  static std::array white_dir = {
    MoveDir{Regular, 1, 2},
    MoveDir{Regular, 1, -2},
    MoveDir{Regular, 2, 1},
    MoveDir{Regular, 2, -1},
    MoveDir{Regular, -1, 2},
    MoveDir{Regular, -1, -2},
    MoveDir{Regular, -2, 1},
    MoveDir{Regular, -2, -1}
  };
}
