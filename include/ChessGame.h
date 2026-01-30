#ifndef CHESSGAME_H
#define CHESSGAME_H
#include <cstdint>
#include <vector>
#include <GameTypes.h>
#include <MoveGenerator.h>



class ChessGame {
public:
  struct GameState {
    Color current_turn{White};
    bool king_moved_w {false};
    bool q_rook_white_moved {false};
    bool k_rook_white_moved {false};
    bool king_moved_b {false};
    bool q_rook_black_moved {false};
    bool k_rook_black_moved {false};
    Square en_passant_target_square{};
    bool passant_sqr_exists{false};
    size_t half_move_clock{};
    size_t full_moves{1};
  };

  ChessGame();
  explicit ChessGame(const std::string& fen);

  ChessGame(ChessGame&&) = default;
  ~ChessGame() = default;
  ChessGame& operator=(ChessGame&&) = delete;
  ChessGame(const ChessGame&) = delete;
  ChessGame& operator=(const ChessGame&) = delete;

  void apply_move(Move move);
  std::vector<Move> generate_legal_moves(Piece p, Square s);
  const std::vector<std::pair<Piece, Square>>& get_piece_list();
  bool is_check(Square s) const;
  void undo_move();
  Color get_current_turn() const;

  GameState get_state() {
    return state;
  }

  void set_state(GameState& s) {
    state = s;
  }

private:
  GameBoard board;
  MoveGenerator move_gen;
  bool is_legal_move(Piece p, Move m);

  void promote_piece(PieceType promote_to, Square s);
  bool can_enpassant(Piece pos, Square s) const;
  bool can_promote(Piece pos, Square s) const;
  void update_en_passant_square(Square last_pawn_move, Color pawn_color);

  void set_castling_from_fen(std::string_view f);
  bool can_k_side_castle(Square king_pos);
  bool can_q_side_castle(Square king_pos);

  void update_castling_rights(Piece p,Square source);
  std::vector<Move> get_castling_squares(Square king_pos);
  std::array<Move, 4> get_promotion_moves(Square from, Square to);
  Move get_rook_castle_move(const Move &move);

  GameState state;
  std::vector<Move> move_history;
  std::vector<Position>& piece_list;
  std::vector<GameState> prev_state;

};


#endif