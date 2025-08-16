#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "board.h"
#include "player.h"
#include "view.h"
#include "graphicalview.h"

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <vector>

class Controller {
    int currentPlayer;
    std::unique_ptr<View> tv;
    std::vector<std::unique_ptr<GraphicalView>> graphicalViews;
    std::unique_ptr<Board> board;

    bool useGraphics;
    bool viewPerPlayer;
    bool POVEnabled;

    std::stack<std::unique_ptr<std::istream>> inputStack;

    bool gameOver();
    bool nextTurn();
    
    void announceWinner(int playerIndex);

    void convertCoordinatesPOV(int& row, int& col, int POVindex);

    public:
        std::vector<Player> players;
        void play();
        Controller(
            int numPlayers,
            bool useGraphics,
            bool viewPerPlayer,
            bool POVEnabled
        );

        void notify(const Tile& tile);
};

#endif
