#include "trapeffect.h"
#include "gamepiece.h"

TrapEffect::TrapEffect(Player* player) : TileEffect(player) {}


void TrapEffect::onEnter(GamePiece* piece) {
    piece->setTrapped(true);
}

void TrapEffect::onLeave(GamePiece* piece) {
    piece->setTrapped(false);
}

bool TrapEffect::isTrap() const {
    return true;
}

