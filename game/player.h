#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "constants.h"
#include "gamepiece.h"

#include <map>
#include <memory>
#include <vector>


class Board;

class Player {
    int index;
    bool isDeleted;
    bool hasWon;
    std::vector<Tile*> trackedTiles;
    void removeOwnedTileEffects();
    void removePieces();

    public:    
        std::map<char, std::unique_ptr<GamePiece>> pieces;
        int getIndex() const;
        void addTrackedTiles(Tile* tile);
        void setHasWon(bool value);
        bool getHasWon() const;
        void deletePlayer();
        Constants::MOVE_RESULT move(Board* board, char pieceId, char dir);
        Player(int index, char startingPiece);
};

#endif