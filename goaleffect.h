#ifndef __GOALEFFECT_H__
#define __GOALEFFECT_H__

#include "tileeffect.h"

class GamePiece;

class GoalEffect : public TileEffect {
    public:
        GoalEffect(Player* player);
        void onEnter(GamePiece* piece) override;
        bool isGoal() const override;
};

#endif