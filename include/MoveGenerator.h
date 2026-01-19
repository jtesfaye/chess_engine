#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H
#include <GameTypes.h>
#include <array>
#include <vector>

class MoveGenerator {
public:
  MoveGenerator(GameBoard& b, const std::vector<MoveChange>& changes)
  : board(b),
  changes(changes) 
  {}

  std::vector<Square> generate_pseudo_legal_moves(Piece p, bool can_kcastle = false, bool can_qcastle = false);
  static bool canEnPassant(Piece current_pos, MoveChange previous_move);
  static bool bound_check(int r, int f);

  static inline std::array knight_dir = {
    MoveDir{1, 2},
    MoveDir{ 1, -2},
    MoveDir{ 2, 1},
    MoveDir{ 2, -1},
    MoveDir{ -1, 2},
    MoveDir{ -1, -2},
    MoveDir{ -2, 1},
    MoveDir{-2, -1}
  };

  static inline std::array bishop_directions = {
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1}
  };

  static inline std::array rook_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1}
  };

  static inline std::array queen_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1},
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1}
  };

  static inline std::array king_directions = {
    MoveDir {1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1},
    MoveDir {1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1},
  };
  
private:

  std::vector<Square> generate_pawn_pseudo_legal_moves(Piece move) const;
  std::vector<Square> generate_knight_pseudo_legal_moves(Piece move) const;
  std::vector<Square> generate_bishop_pseudo_legal_moves(Piece move) const;
  std::vector<Square> generate_rook_pseudo_legal_moves(Piece move) const;
  std::vector<Square> generate_queen_pseudo_legal_mvoes(Piece move) const;
  std::vector<Square> generate_king_pseudo_legal_moves(Piece move, bool can_kcastle, bool can_qcastle) const;

  template<size_t N>
  std::vector<Square> generate_sliding_moves(const std::array<MoveDir, N>& dirs, Piece move) const;

  static Piece intToPiece(u_int8_t pos);

  GameBoard& board;
  const std::vector<MoveChange>& changes;
};

#endif