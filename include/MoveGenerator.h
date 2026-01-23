#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H
#include <GameTypes.h>
#include <array>
#include <vector>

class MoveGenerator {
public:
  explicit MoveGenerator(GameBoard& b)
  : board(b)
  {}

  std::vector<Square> generate_pseudo_legal_moves(Square p);

  static constexpr std::array knight_dir = {
    MoveDir{1, 2},
    MoveDir{ 1, -2},
    MoveDir{ 2, 1},
    MoveDir{ 2, -1},
    MoveDir{ -1, 2},
    MoveDir{ -1, -2},
    MoveDir{ -2, 1},
    MoveDir{-2, -1}
  };

  static constexpr std::array bishop_directions = {
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1}
  };

  static constexpr std::array rook_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1}
  };

  static constexpr  std::array queen_directions = {
    MoveDir{1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1},
    MoveDir{1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1}
  };

   static constexpr std::array king_directions = {
    MoveDir {1, 0},
    MoveDir {-1, 0},
    MoveDir {0,1},
    MoveDir {0, -1},
    MoveDir {1, 1},
    MoveDir {1, -1},
    MoveDir {-1,1},
    MoveDir {-1, -1},
  };

  static constexpr MoveDir k_side_castle_dir{0, 2};
  static constexpr MoveDir q_side_castle_dir{0, -2};

private:

  std::vector<Square> generate_pawn_pseudo_legal_moves(Piece p, Square s) const;
  std::vector<Square> generate_knight_pseudo_legal_moves(Piece p, Square s) const;
  std::vector<Square> generate_bishop_pseudo_legal_moves(Piece p, Square s) const;
  std::vector<Square> generate_rook_pseudo_legal_moves(Piece p, Square s) const;
  std::vector<Square> generate_queen_pseudo_legal_moves(Piece p, Square s) const;
  std::vector<Square> generate_king_pseudo_legal_moves(Piece p, Square s) const;

  template<size_t N>
  std::vector<Square> generate_sliding_moves(const std::array<MoveDir, N>& dirs, Piece p, Square s) const;

  static Piece intToPiece(u_int8_t pos);

  GameBoard& board;
};

#endif