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

  GameBoard& get_board() {
    return board;
  }

  Piece at(Rank r, File f);

private:

  void set_initial_board();

  bool kingMoved;
  bool qSideRookMoved;
  bool kSideRookMoved;

  GameBoard board;
  std::vector<MoveChange> move_history;
};


#endif