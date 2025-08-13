#ifndef __MOVEMENTSYSTEM_H__
#define __MOVEMENTSYSTEM_H__

#include "constants.h"

class Board;
class GamePiece;
class Tile;

class MovementSystem {
    protected:
        GamePiece* piece;
        virtual bool battle(GamePiece* opponent);
        virtual Tile* getDestinationTile(Board& board, char dir) = 0;

    public:
        MovementSystem(GamePiece* piece);
        virtual Constants::MOVE_RESULT move(Board& board, char dir);
        virtual void leaveTile();
        virtual void enterTile(Tile* tile);
        void setGamePiece(GamePiece* piece);
};

#endif
