#ifndef GAMETYPES_H
#define GAMETYPES_H

#include <cstdint>
#include <utility>
#include <vector>
#include <cstddef>
#include <array>

using MoveList = std::vector<std::pair<size_t, size_t>>;
using Board = std::array<std::array<u_int8_t, 8>, 8>;

enum PieceType : u_int8_t {
  NoPiece, Pawn, Knight, Bishop, Rook, Queen, King
};

enum Color : u_int8_t {
  NoColor = 0, White = 8, Black = 16
};

enum Rank : size_t {
  Rank_1, Rank_2, Rank_3, Rank_4, Rank_5, Rank_6, Rank_7, Rank_8
};

enum File : size_t {
  File_A, File_B, File_C, File_D, File_E, File_F, File_G, File_H
};

enum MoveType {
  Regular, Capture
};

inline u_int8_t piece(Color color, PieceType piece) {
  return color | piece;
}

struct MoveDir {
  MoveType type;
  int rank_change;
  int file_change;
};

struct Piece {
    PieceType type;
    Color color;
};

struct Position {
    File f;
    Rank r;
    Piece piece;
};

struct MoveChange {
    Position from;
    Position to;
};

#endif