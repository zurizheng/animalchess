#include "movementsystem.h"

#include "board.h"
#include "constants.h"
#include "player.h"
#include "gamepiece.h"
#include "tileeffect.h"

#include <iostream> // For debugging only


MovementSystem::MovementSystem(GamePiece* piece) : piece(piece) {}


Constants::MOVE_RESULT MovementSystem::move(Board& board, char dir) {
    Tile* currentTile = board.getTile(piece->getRow(), piece->getCol());
    Tile* newTile = getDestinationTile(board, dir);
    Player* owner = piece->getOwner();
    
    if (!newTile) {
        std::cout << "DEBUG: Invalid move, destination tile is null (this shouldn't happen?)." << std::endl;
        return Constants::MOVE_INVALID;
    }
    
    TileEffect* effect = newTile->getTileEffect();

    // Check if moving into opponents goal which the player owns
    // Need to do this before wall check bc goals are walls, but we move into them
    // if it's our own goal
    if (effect && effect->isGoal()) {
        if (effect->getPlayer() == owner) {
            effect->onEnter(piece);
            std::cout << "[INFO] Player " << effect->getPlayer()->getIndex() + 1 << " has reached their goal!" << std::endl;
        }
    }

    // Check if moving into a wall (or own goal)
    if (newTile->getIsWall()) {
        return Constants::MOVE_WALL;
    }

    // Can't move into own trap
    if (effect && effect->isTrap()) {
        if (effect->getPlayer() == owner) {
            return Constants::MOVE_WALL;
        }
    }

    GamePiece* otherPiece = newTile->getPiece();
    Player* otherOwner = otherPiece ? otherPiece->getOwner() : nullptr;

    // Check if moving onto one of our own pieces
    if (otherPiece && otherPiece != piece && otherOwner == owner) {
        return Constants::MOVE_OWNPIECE;
    }

    // Call any onEnter effects (before battle)
    if (effect) {
        effect->onEnter(piece);
        if (piece->isDead()) {
            return Constants::MOVE_KILLED;
        }
    }

    if (otherPiece && otherOwner != owner) {
        battle(otherPiece);
        if (piece->isDead()) {
            return Constants::MOVE_KILLED;
        }
    }

    leaveTile();
    enterTile(newTile);

    return Constants::MOVE_SUCCESS;
}


void MovementSystem::leaveTile() {
    Tile* currentTile = piece->getBoard()->getTile(piece->getRow(), piece->getCol());

    if (currentTile->getTileEffect()) {
        currentTile->getTileEffect()->onLeave(piece);
    }

    piece->setPosition(-1, -1); // Invalid position
    currentTile->setPiece(nullptr);

    // Optional extra behavior when leaving a tile
    // So piece decorators can add their own behavior (SLIME)
    piece->onLeaveTile(currentTile);

    currentTile->notify();
}


void MovementSystem::enterTile(Tile* tile) {
    piece->setPosition(tile->getRow(), tile->getColumn());
    tile->setPiece(piece);
    tile->notify();
}


// returns true if attacker wins, false is defender wins
bool MovementSystem::battle(GamePiece* opponent) {

    // variables for accessing Players
    Player* winner = nullptr;
    Player* loser = nullptr;
    GamePiece* defeatedGamePiece = nullptr;
    GamePiece* winningGamePiece = nullptr;

    // determine strength
    int atkStrength = piece->getStrength();
    int defStrength = opponent->getStrength();

    // unique: mouse beats elephant
    if (atkStrength == 1 && defStrength == 8) {
        // attacker is mouse and defender is elephant, attacker wins
        winner = piece->getOwner();
        loser = opponent->getOwner();
        defeatedGamePiece = opponent;
        winningGamePiece = piece;
    } else if (atkStrength == 8 && defStrength == 1) {
        // attacker is elephant and defender is mouse, defender wins
        winner = opponent->getOwner();
        loser = piece->getOwner();
        defeatedGamePiece = piece;
        winningGamePiece = opponent;
    }
    // compare strength (battling)
    else if (atkStrength >= defStrength) {
        // attacker wins
        winner = piece->getOwner();
        loser = opponent->getOwner();
        defeatedGamePiece = opponent;
        winningGamePiece = piece;
    } else {
        // defender wins
        winner = opponent->getOwner();
        loser = piece->getOwner();
        defeatedGamePiece = piece;
        winningGamePiece = opponent;
    }

    // Winner remains on board
    std::cout << "[INFO] Player " << winner->getIndex() + 1 << " wins battle vs Player " << loser->getIndex() + 1 << std::endl;

    defeatedGamePiece->remove();

    // Notify board of winner (bc. it's revealed, shows in graphics)
    winningGamePiece->getBoard()->notify(
        *winningGamePiece->getBoard()->getTile(winningGamePiece->getRow(), winningGamePiece->getCol())
    );

    // return true if the attacker is the winner
    return winner == piece->getOwner();
}

void MovementSystem::setGamePiece(GamePiece* piece) {
    this->piece = piece;
}




