#ifndef __SERVERPORTEFFECT_H__
#define __SERVERPORTEFFECT_H__

#include "tileeffect.h"

class GamePiece;

class ServerPortEffect : public TileEffect {
    public:
        ServerPortEffect(Player* player);
        virtual void onEnter(GamePiece* piece) override;
        bool isServerPort() const override;
};

#endif