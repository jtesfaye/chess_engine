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

struct Position {
    Square curr_square;
    Piece piece;
};

struct MoveChange {
    Position from;
    Position to;
};

class GameBoard {
  Board board{};
public:
  Piece at(const Rank r, const File f) const;
  void set_piece(const Rank r, const File f, const Piece p);
  void load_board(std::string fen);
  static Piece intToPiece(u_int8_t pos);
  static u_int8_t char_to_piece(char c);
  std::string to_fen_piece_placement() const;

};

#endif