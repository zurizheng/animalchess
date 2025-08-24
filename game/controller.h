#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "board.h"
#include "player.h"
#include "view.h"
#include "graphicalview.h"
#include "../ai/aiplayer.h"
#include "../ai/training_visualizer.h"

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
    bool aiTraining;  // Flag for AI training mode

    std::stack<std::unique_ptr<std::istream>> inputStack;
    std::vector<std::unique_ptr<AIPlayer>> aiPlayers;  // AI players
    std::unique_ptr<TrainingVisualizer> visualizer;    // Training visualization

    bool gameOver();
    bool nextTurn();
    bool handleAITurn();  // Handle AI player turn
    
    void announceWinner(int playerIndex);

    void convertCoordinatesPOV(int& row, int& col, int POVindex);

    public:
        std::vector<Player> players;
        void play();
        void trainAI(int numGames);  // AI training function
        void playAgainstAI();        // Play against trained AI
        Controller(
            int numPlayers,
            bool useGraphics,
            bool viewPerPlayer,
            bool POVEnabled,
            bool aiTraining = false
        );

        void notify(const Tile& tile);
        void setAIPlayer(int playerIndex, double learningRate = 0.001);
        bool isAIPlayer(int playerIndex) const;
};

#endif
