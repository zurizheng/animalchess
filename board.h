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
    int length; // length of the board side length including walls
    int width; // width of the board
    Controller* controller;

    // Helper function to get which player index a char belongs to 
    int getPlayer(char t);
    GamePiece* makeGamePiece(int row, int col, char tileChar, std::vector<Player>& players);


    public:
        bool init(std::vector<std::string> layout, std::vector<Player>& players);
        Tile* getTile(int row, int col);
        int getLength();
        int getWidth();
        void notify(const Tile& tile);
        Board(int length, int width, Controller* controller);
};

#endif
