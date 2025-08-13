#include "serverporteffect.h"
#include "gamepiece.h"

ServerPortEffect::ServerPortEffect(Player* player) : TileEffect(player) {}


void ServerPortEffect::onEnter(GamePiece* piece) {
    piece->download(player);
}


bool ServerPortEffect::isServerPort() const {
    return true;
}

