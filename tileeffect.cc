#include "tileeffect.h"
#include "player.h"

TileEffect::TileEffect(Player* player) : player(player) {}


TileEffect::~TileEffect() = default;


void TileEffect::onEnter(GamePiece* piece) {
    // Dust...
}

void TileEffect::onLeave(GamePiece* piece) {
    // Cobwebs...
}


Player* TileEffect::getPlayer() {
    return player;
}


bool TileEffect::isGoal() const {
    return false; // Default to false
}

bool TileEffect::isTrap() const {
    return false; // Default to false
}

