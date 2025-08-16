#include "board.h"

#include "gamepiece.h"
#include "constants.h"
#include "controller.h"
#include "trapeffect.h"
#include "player.h"
#include "goaleffect.h"
#include "tile.h"
#include "tileeffect.h"

#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <utility>


Board::Board(int size, Controller* controller) : size{size}, controller{controller} {}


bool Board::init(std::vector<std::string> layout, std::vector<Player>& players) {
    // Avoid resizes to prevent reallocations
    size_t height = layout.size();

    // Board layout must match specified size
    assert(height == size);

    board.reserve(height);

    for (int r = 0; r < height; r++) {
        size_t width = layout[r].size();

        // Avoid reallocations
        board.emplace_back();
        board[r].reserve(width);

        for (int c = 0; c < width; c++) {
            char tileChar = layout[r][c];
            
            bool isWall =  (tileChar == 'X');
            bool isWater = (tileChar == 'W');
            bool isTrap =  (tileChar == 'T');
            bool isGoal =  (tileChar == 'G');

            std::unique_ptr<TileEffect> tileEffect = nullptr;
            
            int playerIndex = (r <= 6) ? 0 : 1;
            Player* player = &players[playerIndex];
            
            if (isGoal) {
                // make the goal be owned by the player who is trying to get to it
                tileEffect = std::make_unique<GoalEffect>(&players[(playerIndex + 1) % 2]);
            }
            else if (isTrap) {
                tileEffect = std::make_unique<TrapEffect>(player);
            }
            
            GamePiece* piece = nullptr;
            
            // check if integer
            if (std::isdigit(tileChar)) {
                piece = makeGamePiece(r, c, tileChar, players);
            }

            // Create Tile object
            board[r].emplace_back(
                r, c,
                piece,
                std::move(tileEffect),
                isWall,
                isWater,
                this
            );

            // player aware of this tile if it is server or endpoint
            if (board[r][c].getTileEffect()) {
                board[r][c].getTileEffect()->getPlayer()->addTrackedTiles(&board[r][c]);
            }

            // Notify the controller of the new tile
            notify(board[r][c]);
        }
    }

    return true;
}

GamePiece* Board::makeGamePiece(int row, int col, char tileChar, std::vector<Player>& players) {
    if (tileChar < '1' && '8' > tileChar) return nullptr;

    int playerIndex = (row <= 6) ? 0 : 1;

    Player& player = players[playerIndex];

    player.pieces[tileChar] = std::make_unique<GamePiece>(
        tileChar, row, col, tileChar - '0', &player, this
    );

    return player.pieces[tileChar].get();
}


void Board::notify(const Tile& tile) {
    controller->notify(tile);
}


Tile* Board::getTile(int row, int col) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
        return nullptr; // Out of bounds
    }
    return &board[row][col];
}

int Board::getSize() {
    return size;
}


