#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "constants.h"
#include "gamepiece.h"

#include <map>
#include <memory>
#include <vector>


class Board;

class Player {
    int downloadedData;
    int downloadedViruses;
    int index;
    bool isDeleted;
    std::vector<Tile*> trackedTiles;
    void removeOwnedTileEffects();
    void removePieces();

    public:    
        std::map<char, std::unique_ptr<GamePiece>> pieces;
        int getIndex() const;
        void addTrackedTiles(Tile* tile);
        void deletePlayer();
        Constants::MOVE_RESULT move(Board* board, char pieceId, char dir);
        bool isWinner() const;
        bool isLoser() const;
        Player(int index, char startingPiece);
};

#endif