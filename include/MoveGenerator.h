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

  std::vector<Square> generate_pseudo_legal_moves(Position p);
  static bool canEnPassant(Position current_pos, MoveChange previous_move);
  
private:

  std::vector<Square> generate_pawn_pseudo_legal_moves(Position move) const;
  std::vector<Square> generate_knight_pseudo_legal_moves(Position move) const;
  std::vector<Square> generate_bishop_pseudo_legal_moves(Position move) const;
  std::vector<Square> generate_rook_pseudo_legal_moves(Position move) const;
  std::vector<Square> generate_queen_pseudo_legal_mvoes(Position move) const;
  std::vector<Square> generate_king_pseudo_legal_moves(Position move, bool kside_castle, bool qside_castle) const;

  template<size_t N>
  std::vector<Square> generate_sliding_moves(const std::array<MoveDir, N>& dirs, const Position move) const;

  static Piece intToPiece(u_int8_t pos);

  GameBoard& board;
  const std::vector<MoveChange>& changes;
};

#endif