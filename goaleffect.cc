#include "goaleffect.h"
#include "gamepiece.h"
#include "player.h"

GoalEffect::GoalEffect(Player* player) : TileEffect(player) {}


void GoalEffect::onEnter(GamePiece* piece) {
    piece->remove();
    piece->getOwner()->setHasWon(true);
}


bool GoalEffect::isGoal() const {
    return true;
}

