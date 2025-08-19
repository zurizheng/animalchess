#ifndef __WATERMOVE_H__
#define __WATERMOVE_H__

#include "movementsystem.h"
#include "constants.h"


class Board;
class Tile;


class WaterMove : public MovementSystem {
    bool shouldStop(Board& board, int row, int col);
    
    public:
        WaterMove(GamePiece* piece);
        virtual Tile* getDestinationTile(Board& board, char dir) override;
};

#endif
