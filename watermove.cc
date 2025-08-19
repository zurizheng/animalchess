#include "watermove.h"
#include "board.h"
#include "constants.h"
#include "gamepiece.h"
#include "player.h"
#include "tile.h"
#include "tileeffect.h"


WaterMove::WaterMove(GamePiece* piece) : MovementSystem(piece) {}


bool WaterMove::shouldStop(Board& board, int row, int col) {
    Tile* tile = board.getTile(row, col);
    return tile->getIsWall();
}


// dir takes in cardinal directions
Tile* WaterMove::getDestinationTile(Board& board, char dir) {
    int speed = 1;
    int row = piece->getRow();
    int col = piece->getCol();

    int horizontal = 0;
    int vertical = 0;
    switch (std::toupper(dir)) {
        case 'N': vertical = -1; break;
        case 'S': vertical = 1; break;
        case 'W': horizontal = -1; break;
        case 'E': horizontal = 1; break;
        default:
            // Invalid input (shouldn't happen)
            return nullptr;
    }

    int endRow = row;
    int endCol = col;

    // check the conditions one step at a time
    for (int i = 0; i < speed; i++) {
        endRow += vertical;
        endCol += horizontal;

        if (shouldStop(board, endRow, endCol)) {
            break;
        }
    }

    // Shouldn't ever be out of bounds because walls prevent this
    return board.getTile(endRow, endCol);
}

