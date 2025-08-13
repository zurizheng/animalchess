#ifndef __BOARD_H__
#define __BOARD_H__

#include <vector>
#include <map>
#include <string>
#include "tile.h"

class Controller;
class Player;
class GamePiece;

class Board {
    std::vector<std::vector<Tile>> board;
    int size; // Size of the board side length including walls
    Controller* controller;

    // Helper function to get which player index a char belongs to 
    int getPlayer(char t);
    GamePiece* makeGamePiece(int row, int col, char tileChar, std::vector<Player>& players, 
                   std::vector<std::vector<std::pair<bool, int>>>& playerPlacements);


    public:
        bool init(std::vector<std::string> layout, std::vector<Player>& players,
                  std::vector<std::vector<std::pair<bool, int>>>& playerPlacements);
        Tile* getTile(int row, int col);
        int getSize();
        void notify(const Tile& tile);
        Board(int size, Controller* controller);
};

#endif
