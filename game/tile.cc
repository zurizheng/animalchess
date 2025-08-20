#include "gamepiece.h"

#include "board.h"
#include "tileeffect.h"

#include <memory>


Tile::Tile(int row, int col, 
            GamePiece* piece, 
            std::unique_ptr<TileEffect> effect, 
            bool isWall,
            bool isWater,
            Board* board)
    : row{row}, col{col}, 
      piece{piece}, 
      effect{std::move(effect)}, 
      isWall{isWall},
      isWater{isWater},
      board{board} {}

int Tile::getRow() const {
    return row;
}

int Tile::getColumn() const {
    return col;
}

GamePiece* Tile::getPiece() const {
    return piece;
}

void Tile::setPiece(GamePiece* piece) {
    this->piece = piece;
}

bool Tile::getIsWall() const {
    return isWall;
}

bool Tile::getIsWater() const {
    return isWater;
}

TileEffect* Tile::getTileEffect() const {
    return effect.get();
}

void Tile::setTileEffect(std::unique_ptr<TileEffect> effect) {
    this->effect = std::move(effect);
}

void Tile::notify() const {
    board->notify(*this);
}



