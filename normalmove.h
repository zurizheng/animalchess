#ifndef __NORMALMOVE_H__
#define __NORMALMOVE_H__

#include "movementsystem.h"
#include "constants.h"


class Board;
class Tile;


class NormalMove : public MovementSystem {
    bool shouldStop(Board& board, int row, int col);
    
    public:
        NormalMove(GamePiece* piece);
        virtual Tile* getDestinationTile(Board& board, char dir) override;
};

#endif
