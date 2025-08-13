#ifndef __ENDPOINTEFFECT_H__
#define __ENDPOINTEFFECT_H__

#include "tileeffect.h"

class GamePiece;

class EndpointEffect : public TileEffect {
    public:
        EndpointEffect(Player* player);
        virtual void onEnter(GamePiece* piece) override;
        bool isEndpoint() const override;
};

#endif
