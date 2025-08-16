#ifndef __GRAPHICALVIEW_H__
#define __GRAPHICALVIEW_H__

#include "view.h"
#include "window.h"

#include <vector>


struct TileInfo {
    bool initialized = false;
    bool isWall;
    bool hasGamePiece;
    int pieceOwner;
    char piece;
    int strength;
    bool hasTileEffect;
    bool isGoal;
    bool isTrap;
    bool isWater;
    int effectOwner;
};

class GraphicalView : public View {
    std::vector<std::vector<int>> playerColors;
    std::vector<std::vector<int>> serverPortColors;

    int currentPlayer = 0;
    int numPlayers;
    int screenIndex;
    
    int boardDimension;
    int playerInfoHeight, playerInfoWidth;
    int boardX, boardY;
    
    int playerInfoX;
    int playerInfo1Y, playerInfo2Y;
    
    int infoHeight, infoWidth;
    int infoX, infoY;
    
    int windowHeight, windowWidth;
    
    Xwindow window;
    bool displayReady;
    bool POVEnabled;

    std::vector<std::vector<TileInfo>> grid;
    std::pair<int, int> accessTileInfo(int row, int col);
    void convertCoordinatesPOV(int& row, int& col, int POVindex);
    void drawPlayerInfo(int x, int y, int width, int height, int playerIndex, bool show);
    void updateGrid(const Tile& tile);
    bool checkIsReady();
    void drawTile(const Tile& tile);
    void drawTile(int x, int y, int width, int height, const TileInfo& tileInfo);
    

    public:
        GraphicalView(int boardSize, int screenIndex, const std::vector<Player>& players, bool povEnabled);
        void notify(const Tile& tile) override;
        void print(std::ostream& out, int playerIndex, bool POV = false) override;
        void printWhoWon(std::ostream& out, int playerIndex) override;
        void redrawPlayers();
        void drawWholeBoard();
};

#endif

