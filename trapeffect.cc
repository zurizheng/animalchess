#include "trapeffect.h"
#include "gamepiece.h"

TrapEffect::TrapEffect(Player* player) : TileEffect(player) {}


void TrapEffect::onEnter(GamePiece* piece) {
    piece->remove();
}

void TrapEffect::onLeave(GamePiece* piece) {
    piece->remove();
}

bool TrapEffect::isTrap() const {
    return true;
}

