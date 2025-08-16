#ifndef __TRAPEFFECT_H__
#define __TRAPEFFECT_H__

#include "tileeffect.h"

class GamePiece;

class TrapEffect : public TileEffect {
    public:
        TrapEffect(Player* player);
        void onEnter(GamePiece* piece) override;
        void onLeave(GamePiece* piece) override;
        bool isTrap() const override;
};

#endif