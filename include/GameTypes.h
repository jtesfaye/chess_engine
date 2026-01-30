#ifndef GAMETYPES_H
#define GAMETYPES_H

#include <cstdint>
#include <utility>
#include <vector>
#include <cstddef>
#include <array>
#include <iostream>
#include <sstream>
#include <__format/format_functions.h>

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

inline std::string file_to_string(File f) {
  switch (f) {
    case File_A:
      return "a";
    case File_B:
      return "b";
    case File_C:
      return "c";
    case File_D:
      return "d";
    case File_E:
      return "e";
    case File_F:
      return "f";
    case File_G:
      return "g";
    case File_H:
      return "h";
    default:
      return "";
  }
}

enum MoveType {
  Regular, Capture
};

inline u_int8_t piece(Color color, PieceType piece) {
  return color | piece;
}

struct Square {
  Rank rank;
  File file;

  std::string to_string() const {
    return std::format("{}{}", file_to_string(file), static_cast<int>(rank) + 1);
  }
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

  [[nodiscard]] bool is_castling() const {
    return is_k_castle || is_q_castle;
  }
};

struct MoveChange {
  Move move;
  Piece was_captured{NoPiece, NoColor};
  Square captured_square;

};

using Position = std::pair<Piece, Square>;

class GameBoard {
  Board board{};
public:

  GameBoard();
  GameBoard(std::string fen_str);
  Piece at(Rank r, File f) const;
  Piece at(Square s) const;
  Piece piece_at(Square s) const;
  Position position_at(Square s);


  bool move_piece(Piece p, Move m);
  bool undo_last_move();
  static bool is_inbound(int r, int f);

  static Piece intToPiece(u_int8_t pos);
  Piece char_to_piece(char c);
  std::string to_fen_piece_placement() const;
  std::vector<Position>& get_piece_list();
  bool set_piece(Piece p, Square s);
  bool capture_piece(Square s);

  void print_board() const {
    std::cout << std::endl;
    for (int r = Rank_8; r >= Rank_1; --r) {
      std::cout << (r + 1) << "  ";
      for (int f = File_A; f <= File_H; ++f) {
        Piece p = at(static_cast<Rank>(r), static_cast<File>(f));
        std::cout << piece_to_fen_char(p) << ' ';
      }
      std::cout << '\n';
    }

    std::cout << "\n   a b c d e f g h\n";
  }

private:

  bool clear_piece(Square s);
  void update_piece_position(Square from, Square to);
  std::vector<std::pair<Piece, Square>> load_from_fen_piece_placement(std::string fen);
  void set_initial_board();
  bool is_regular_capture(Piece p, Square to_squre);

  static char piece_to_fen_char(Piece p) {
    if (p.type == NoPiece) {
      return '.';
    }

    char c;
    switch (p.type) {
      case Pawn:   c = 'p'; break;
      case Knight: c = 'n'; break;
      case Bishop: c = 'b'; break;
      case Rook:   c = 'r'; break;
      case Queen:  c = 'q'; break;
      case King:   c = 'k'; break;
      default:     c = '?'; break;
    }

    if (p.color == White) {
      c = static_cast<char>(std::toupper(c));
    }

    return c;
  }

  std::vector<Position> piece_list;
  std::vector<MoveChange> move_history;
};

#endif