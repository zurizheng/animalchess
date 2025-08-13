#include "gamepiece.h"

#include "link.h"
#include "movementsystem.h"
#include "normalmove.h"
#include "player.h"

#include <memory>


GamePiece::GamePiece(char piece, int row, int col, int strength, Player* owner, Board* board)
    : piece{piece}, row{row}, col{col}, strength{strength}, dead{false}, trapped{false}, owner{owner}, board{board} { // default values
    movementSystem = std::make_unique<NormalMove>(this);
}


int GamePiece::getRow() {
    return row;
}

int GamePiece::getCol() {
    return col;
}

void GamePiece::setPosition(int row, int col) {
    this->row = row;
    this->col = col;
}


bool GamePiece::download(Player* player) {
    if (dead) {
        return false;
    }

    movementSystem->leaveTile();
    dead = true;

    return true;
}

bool GamePiece::isDead() {
    return dead;
}

bool GamePiece::isTrapped() {
    return trapped;
}

int GamePiece::getStrength() {
    return strength;
}

char GamePiece::getPiece() {
    return piece;
}

Board* GamePiece::getBoard() {
    return board;
}

Player* GamePiece::getOwner() {
    return owner;
}

MovementSystem* GamePiece::getMovementSystem() {
    return movementSystem.get();
}

void GamePiece::setMovementSystem(std::unique_ptr<MovementSystem> movementSystem) {
    this->movementSystem = std::move(movementSystem);
}

void GamePiece::onLeaveTile(Tile* tile) {}

