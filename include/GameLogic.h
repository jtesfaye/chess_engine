#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include <cstdint>
#include <vector>
#include <GameTypes.h>

class GameLogic {
public:
  GameLogic();
  GameLogic(GameLogic&&) = default;
  GameLogic& operator=(GameLogic&&) = default;
  ~GameLogic() = default;

  GameLogic(const GameLogic&) = delete;
  GameLogic& operator=(const GameLogic&) = delete;

  inline Board& get_board() {
    return board;
  }

private:
  void set_initial_board();
  Piece intToPiece(u_int8_t pos);

  bool kingMoved;
  bool qSideRookMoved;
  bool kSideRookMoved;

  Board board{};
  std::vector<MoveChange> move_history;
};

#endif