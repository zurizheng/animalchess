#ifndef __GAMEPIECE_H__
#define __GAMEPIECE_H__

#include "link.h"
#include "movementsystem.h"

#include <memory>


class Player;
class Tile;


class GamePiece {
    char piece;
    int row, col;
    int strength;
    bool dead;
    bool trapped;

    Player* owner;
    Board* board;
    std::unique_ptr<MovementSystem> movementSystem;

    public:
        GamePiece(char piece, int row, int col, int strength, Player* owner, Board* board);

        int getRow();
        int getCol();
        void setPosition(int row, int col);

        bool remove();
        
        bool isDead();
        bool isTrapped();
        void setTrapped(bool state);

        int getStrength();
        char getPiece();
        Board* getBoard();

        Player* getOwner();

        MovementSystem* getMovementSystem();
        
        // Pass in new movementSystem*, owns-a
        void setMovementSystem(std::unique_ptr<MovementSystem> movementSystem);

        void onLeaveTile(Tile* tile);
};

#endif