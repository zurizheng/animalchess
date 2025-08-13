#include "board.h"

#include "gamepiece.h"
#include "constants.h"
#include "controller.h"
#include "endpointeffect.h"
#include "player.h"
#include "serverporteffect.h"
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


bool Board::init(std::vector<std::string> layout, std::vector<Player>& players, 
                 std::vector<std::vector<std::pair<bool, int>>>& playerPlacements) {
    // Avoid resizes to prevent reallocations
    size_t height = layout.size();

    // Board layout must match specified size
    assert(height == size);

    board.reserve(height);

    for (int r = 0; r < height; r++) {
        size_t width = layout[r].size();

        // Board must be square
        assert(width == height);

        // Avoid reallocations
        board.emplace_back();
        board[r].reserve(width);

        for (int c = 0; c < width; c++) {
            char tileChar = layout[r][c];
            
            bool isWall = (tileChar == 'X');

            bool isEndpoint = false;
            std::unique_ptr<TileEffect> tileEffect = nullptr;
            
            // Set endpoint / server ports (indicated by numbers in layout)
            // Endpoints are on the edges of the board, otherwise server ports
            if (std::isdigit(tileChar)) {
                int playerIndex = tileChar - '0';
                Player* player = &players[playerIndex];
                bool isOnEdge = (r == 0 || r == (height - 1) || c == 0 || c == (width - 1));
                isEndpoint = isOnEdge;
                if (isEndpoint) {
                    isWall = true; // endpoints are also walls
                    tileEffect = std::make_unique<EndpointEffect>(player);
                }
                else {
                    tileEffect = std::make_unique<ServerPortEffect>(player);
                }
            }

            GamePiece* piece = makeGamePiece(r, c, tileChar, players, playerPlacements);

            // Create Tile object
            board[r].emplace_back(
                r, c,
                piece,
                std::move(tileEffect),
                isWall,
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


int Board::getPlayer(char t) {
    if (!std::isalpha(t)) {
        return -1;
    }

    const int NUM_PLAYERS = 4;

    // Get player index based on if it's within the range of starting pieces for each player
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (Constants::PLAYER_STARTING_PIECES[i] <= t && 
            t < Constants::PLAYER_STARTING_PIECES[i] + Constants::NUM_LINKS) {
            return i;
        }
    }

    // Not a valid player character
    return -1; 
}


GamePiece* Board::makeGamePiece(int row, int col, char tileChar, std::vector<Player>& players, 
                   std::vector<std::vector<std::pair<bool, int>>>& playerPlacements) {
    int playerIndex = getPlayer(tileChar);
    if (playerIndex < 0 || playerIndex >= players.size()) {
        return nullptr; // Invalid player index
    }


    int linkIndex = tileChar - Constants::PLAYER_STARTING_PIECES[playerIndex];
    Player& player = players[playerIndex];

    std::vector<std::pair<bool, int>>& placements = playerPlacements[playerIndex];
    std::pair<bool, int> placement = placements[linkIndex];

    bool isVirus = placement.first;
    int strength = placement.second;

    player.pieces[tileChar] = std::make_unique<GamePiece>(
        tileChar, row, col, strength, &player, this
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


