#ifndef __TILE_H__
#define __TILE_H__

#include "tileeffect.h"

#include <memory>

class GamePiece;
class Board;

class Tile {
    int row, col;
    GamePiece* piece;
    Board* board;
    bool isWall;
    bool isWater;
    std::unique_ptr<TileEffect> effect;
    
    public:
        Tile(int row, int col, 
            GamePiece* piece, 
            std::unique_ptr<TileEffect> effect,
            bool isWall,
            bool isWater,
            Board* board);


        // getters and setters
        GamePiece* getPiece() const;
        void setPiece(GamePiece* piece);
        TileEffect* getTileEffect() const;
        void setTileEffect(std::unique_ptr<TileEffect> effect);
        bool getIsWall() const;
        bool getIsWater() const;
        int getRow() const;
        int getColumn() const;
        void notify() const;
};

#endif
