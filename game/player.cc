#include "player.h"
#include "board.h"
#include "movementsystem.h"
#include "constants.h"
#include "tile.h"
#include "tileeffect.h"


Player::Player(int index, char startingPiece)
    : index{index}, isDeleted{false} {
    for (char c = startingPiece; c < startingPiece + Constants::NUM_PIECES; c++) {
        pieces[c] = nullptr;
    }
}

int Player::getIndex() const {
    return index;
}

void Player::addTrackedTiles(Tile* tile) {
    trackedTiles.push_back(tile);
}

void Player::removeOwnedTileEffects() {
    TileEffect* effect;
    for (auto tile : trackedTiles) {
        effect = tile->getTileEffect();
        if (effect && effect->getPlayer() == this) {
            // player owns this tile effect, remove it
            tile->setTileEffect(nullptr);
            tile->notify();
        }
    }
}

void Player::removePieces() {
    for (auto& piece : pieces) {
        piece.second.get()->remove();
    }
}

void Player::setHasWon(bool value) {
    hasWon = value;
}

bool Player::getHasWon() const {
    return hasWon;
}

void Player::deletePlayer() {
    if (isDeleted) return;
    removeOwnedTileEffects();
    removePieces();
    isDeleted = true;
}

Constants::MOVE_RESULT Player::move(Board* board, char pieceId, char dir) {
    if (pieces[pieceId]->isDead()) {
        return Constants::MOVE_INVALID;
    }

    return pieces[pieceId]->getMovementSystem()->move(*board, dir);
}
