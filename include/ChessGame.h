#ifndef CHESSGAME_H
#define CHESSGAME_H
#include <cstdint>
#include <vector>
#include <GameTypes.h>
#include <MoveGenerator.h>

class ChessGame {
public:

  ChessGame();
  explicit ChessGame(const std::string& fen);

  ChessGame(ChessGame&&) = default;
  ~ChessGame() = default;
  ChessGame& operator=(ChessGame&&) = delete;
  ChessGame(const ChessGame&) = delete;
  ChessGame& operator=(const ChessGame&) = delete;

  GameBoard& get_board() {
    return board;
  }

  bool is_check(const GameBoard& board, const std::vector<Piece>& list, Color curr_turn) const;

private:

  MoveGenerator move_gen;
  void set_initial_board();
  //bool is_legal_move(MoveChange move);
  void set_castling_from_fen(std::string_view f);

  Color current_turn{White};
  bool king_moved_w {false};
  bool king_moved_b {false};
  bool q_rook_white {false};
  bool k_rook_white {false};
  bool q_rook_black {false};
  bool k_rook_black {false};

  GameBoard board;
  std::vector<MoveChange> move_history;
  std::vector<Piece> piece_list;
  size_t half_move_clock{};
  size_t full_moves{1};
};


#endif