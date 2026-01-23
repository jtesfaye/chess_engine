#ifndef GAMETYPES_H
#define GAMETYPES_H

#include <cstdint>
#include <utility>
#include <vector>
#include <cstddef>
#include <array>
#include <sstream>

using Board = std::array<std::array<u_int8_t, 8>, 8>;


enum PieceType : uint8_t {
  NoPiece, Pawn, Knight, Bishop, Rook, Queen, King
};

enum Color : uint8_t {
  NoColor = 0, White = 8, Black = 16
};

enum Rank {
  Rank_1, Rank_2, Rank_3, Rank_4, Rank_5, Rank_6, Rank_7, Rank_8
};

enum File {
  File_A, File_B, File_C, File_D, File_E, File_F, File_G, File_H
};

enum MoveType {
  Regular, Capture
};

inline u_int8_t piece(Color color, PieceType piece) {
  return color | piece;
}

struct Square {
  Rank rank;
  File file;
};

struct MoveDir {
  int to_rank;
  int to_file;
};

struct Piece {
  PieceType type;
  Color color;
};

struct Move {
  Square from;
  Square to;
  bool is_en_passant{false};
  bool is_k_castle{false};
  bool is_q_castle{false};
  bool needs_pawn_promotion {false};
  PieceType promote_to {NoPiece};

  bool is_castling() const {
    return is_k_castle || is_q_castle;
  }
};

struct MoveChange {
  Move move;
  Piece was_captured{NoPiece, NoColor};
};

using Position = std::pair<Piece, Square>;

class GameBoard {
  Board board{};
public:

  GameBoard();
  GameBoard(std::string fen_str);
  Piece at(Rank r, File f) const;
  Piece at(Square s) const;

  bool move_piece(Piece p, Square from, Square to);
  bool undo_last_move();
  static bool is_inbound(int r, int f);

  static Piece intToPiece(u_int8_t pos);
  Piece char_to_piece(char c);
  std::string to_fen_piece_placement() const;
  std::vector<Position>& get_piece_list();
  bool set_piece(Piece p, Square s);
  bool remove_piece(Square s);


private:

  bool remove_from_piece_list(Square s);
  std::vector<std::pair<Piece, Square>> load_from_fen_piece_placement(std::string fen);
  void set_initial_board();
  bool is_capture(Piece p, Square to_squre);
  std::vector<Position> piece_list;
  std::vector<MoveChange> move_history;
};

#endif