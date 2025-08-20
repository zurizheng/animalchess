#include "leapmove.h"
#include "board.h"
#include "constants.h"
#include "tile.h"
#include "tileeffect.h"
#include "gamepiece.h"

LeapMove::LeapMove(GamePiece* piece) : MovementSystem(piece) {}

Tile* LeapMove::getDestinationTile(Board& board, char dir) {
    int speed = 1;
    int n = board.getLength();
    int m = board.getWidth();

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

    row += vertical;
    col += horizontal;
    // check if water then leaping activates, also check if there is a rat or any piece in general
    while (board.getTile(row, col)->getIsWater() && !board.getTile(row, col)->getPiece()) {
        row += vertical;
        col += horizontal;
    }

    // Shouldn't ever be out of bounds since there is phasing
    return board.getTile(row, col);
}
