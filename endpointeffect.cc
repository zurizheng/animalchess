#include "endpointeffect.h"
#include "gamepiece.h"

EndpointEffect::EndpointEffect(Player* player) : TileEffect(player) {}


void EndpointEffect::onEnter(GamePiece* piece) {
    // the player in the endpoint would be the 
    piece->download(player);
}


bool EndpointEffect::isEndpoint() const {
    return true;
}


