#ifndef __TILEEFFECT_H__
#define __TILEEFFECT_H__

class GamePiece;
class Player;

class TileEffect {
    protected:
        Player* player;
    
    public:
        TileEffect(Player* player);
        virtual ~TileEffect() = 0; // Abstract class

        virtual void onEnter(GamePiece* piece);
        virtual void onLeave(GamePiece* piece);
        Player* getPlayer();

        virtual bool isTrap() const;
        virtual bool isGoal() const;
};

#endif
