#include "gamepiece.h"

#include "board.h"
#include "tileeffect.h"

#include <memory>


Tile::Tile(int row, int col, 
           GamePiece* piece, 
           std::unique_ptr<TileEffect> effect, 
           bool isWall, 
           Board* board)
    : row{row}, col{col}, 
      piece{piece}, 
      effect{std::move(effect)}, 
      isWall{isWall}, 
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

TileEffect* Tile::getTileEffect() const {
    return effect.get();
}

void Tile::setTileEffect(std::unique_ptr<TileEffect> effect) {
    this->effect = std::move(effect);
}

void Tile::notify() const {
    board->notify(*this);
}



