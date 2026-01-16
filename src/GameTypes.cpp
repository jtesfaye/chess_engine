//
// Created by jeremiah tesfaye on 1/15/26.
//

#include <GameTypes.h>

Piece GameBoard::at(const Rank r, const File f) const {
  u_int8_t piece = board[r][f];
  return {
    static_cast<PieceType>(piece & 0x7),
    static_cast<Color>(piece & 0b11000)
  };
};

void GameBoard::set_piece(const Rank r, const File f, const Piece p) {
  board[r][f] = piece(p.color, p.type);
}

void GameBoard::load_board(std::string fen) {
  std::memset(board.data(), 0, sizeof(board));
  std::stringstream b{fen};
  std::vector<std::string> ranks;
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
        board[i][pos] = char_to_piece(c);
        ++ pos;
      }
    }
  }
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

u_int8_t GameBoard::char_to_piece(char c) {
  switch (c) {
    case 'r':
      return piece(Black, Rook);
    case 'n':
      return piece(Black, Knight);
    case 'b':
      return piece(Black, Bishop);
    case 'k':
      return piece(Black, King);
    case 'q':
      return piece(Black, Queen);
    case 'p':
      return piece(Black, Pawn);
    case 'R':
      return piece(White, Rook);
    case 'N':
      return piece(White, Knight);
    case 'B':
      return piece(White, Bishop);
    case 'K':
      return piece(White, King);
    case 'Q':
      return piece(White, Queen);
    case 'P':
      return piece(White, Pawn);
    default:
      return 0;
  }
}