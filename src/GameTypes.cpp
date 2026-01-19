//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <GameTypes.h>
#include <cassert>

Piece GameBoard::at(const Rank r, const File f) const {
  u_int8_t piece = board[r][f];
  return {
    static_cast<PieceType>(piece & 0x7),
    static_cast<Color>(piece & 0b11000)
  };
};

void GameBoard::set_piece(const Piece p) {
  board[p.position.rank][p.position.file] = piece(p.color, p.type);
}

std::vector<Piece> GameBoard::load_from_fen_piece_placement(std::string fen) {
  std::memset(board.data(), 0, sizeof(board));
  std::stringstream b{fen};
  std::vector<std::string> ranks;
  std::vector<Piece> piece_list;
  std::string r;
  while (std::getline(b, r, '/')) {
    ranks.push_back(std::move(r));
  }
  std::reverse(ranks.begin(), ranks.end());
  for (int i{}; i < ranks.size(); i++) {
    std::string_view rank = ranks[i];
    size_t pos{};
    for (const auto& c : rank) {
      if (isdigit(c)) {
        pos += c - '0';
      } else {
        piece_list.emplace_back(char_to_piece(c, static_cast<Rank>(i), static_cast<File>(pos)));
        set_piece(piece_list.back());
        ++ pos;
      }
    }
  }
  return piece_list;
}

Piece GameBoard::intToPiece(u_int8_t pos) {
  return {
    static_cast<PieceType>(pos & 0x7),
    static_cast<Color>(pos & 0b11000)
  };
}

std::string GameBoard::to_fen_piece_placement() const {
  std::ostringstream out;
  for (int rank = Rank_8; rank >= Rank_1; --rank) {
    int empty = 0;
    for (size_t file = File_A; file <= File_H; ++file) {
      uint8_t p = board[rank][file];
      if (p == 0) {
        ++empty;
      } else {
        if (empty > 0) {
          out << empty;
          empty = 0;
        }
        Piece piece = intToPiece(p);
        char c;
        switch (piece.type) {
          case Pawn:   c = 'p'; break;
          case Knight: c = 'n'; break;
          case Bishop: c = 'b'; break;
          case Rook:   c = 'r'; break;
          case Queen:  c = 'q'; break;
          case King:   c = 'k'; break;
          default:     c = '?'; break;
        }
        if (piece.color == White) {
          c = std::toupper(c);
        }
        out << c;
      }
    }
    if (empty > 0) {
      out << empty;
    }
    if (rank != Rank_1) {
      out << '/';
    }
  }
  return out.str();
}

Piece GameBoard::char_to_piece(char c, Rank r, File f) {
  Square s{r, f};
  switch (c) {
    case 'r':
      return {Rook, Black, s};
    case 'n':
      return {Knight, Black, s};
    case 'b':
      return {Bishop, Black, s};
    case 'k':
      return {King, Black, s};
    case 'q':
      return {Queen, Black, s};
    case 'p':
      return {Pawn, Black, s};
    case 'R':
      return {Rook, White, s};
    case 'N':
      return {Knight, White, s};
    case 'B':
      return {Bishop, White, s};
    case 'K':
      return {King, White, s};
    case 'Q':
      return {Queen, White, s};
    case 'P':
      return {Pawn, White, s};
    default:
      return {NoPiece, NoColor, s};
  }
}

void GameBoard::undo_piece(Rank r, File f, Piece p) {
  board[r][f] = 0;
}
