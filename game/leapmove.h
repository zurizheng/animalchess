#ifndef __LEAPMOVE_H__
#define __LEAPMOVE_H__

#include "movementsystem.h"
#include "constants.h"
#include <utility>

class Board;
class Tile;


class LeapMove : public MovementSystem {
    public:
        LeapMove(GamePiece* piece);
        virtual Tile* getDestinationTile(Board& board, char dir) override;
};

#endif
