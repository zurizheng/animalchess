#include "graphicalview.h"

#include "constants.h"
#include "player.h"
#include "trapeffect.h"
#include "goaleffect.h"
#include "tile.h"
#include "window.h"

#include <sstream>
#include <string>
#include <vector>

#include <iostream>

#define LINE_HEIGHT 20
#define SPACING 10
#define TILE_SIZE 50


GraphicalView::GraphicalView(int boardLength, int boardWidth, int screenIndex, const std::vector<Player>& players, bool POVEnabled) :
    View{ players },
    displayReady{ false }, screenIndex{ screenIndex }, POVEnabled{POVEnabled},
    numPlayers{ 2 },
    boardDimension{ (boardLength - 2) * TILE_SIZE }, // -2 for walls
    playerInfoHeight{ LINE_HEIGHT * 5 },
    playerInfoWidth{ boardDimension },
    boardX{ SPACING },
    boardY{ playerInfoHeight * (numPlayers/2) + SPACING * ((numPlayers/2) + 1) },
    playerInfoX{ SPACING },
    playerInfo1Y{ SPACING },
    playerInfo2Y{ boardY + boardDimension + SPACING },
    infoHeight{ LINE_HEIGHT + 10 },
    infoWidth{ boardDimension },
    infoX{ SPACING },
    infoY{ playerInfo2Y + (playerInfoHeight + SPACING) * (numPlayers/2) },
    windowWidth{ boardDimension + (SPACING * 2) },
    windowHeight{
        boardDimension
        + playerInfoHeight * numPlayers
        + infoHeight
        + (numPlayers + 3) * SPACING // Spacing between every section + margins
    },
    window{ windowWidth, windowHeight }
{
    playerColors = {
        {121, 247, 234}, // light teal #79f7ea
        {242, 145, 236}, // light pink #f291ec
        // {144, 178, 245}, // light blue #90b2f5
        // {240, 209, 110} // light mustard yellow #f0d16e
    };

    serverPortColors = {
        {210, 252, 247}, // lighter teal #d2fcf7
        {255, 230, 254}, // lighter pink #ffe6fe
        // {214, 228, 255}, // lighter blue #d6e4ff
        // {255, 244, 209} // lighter mustard yellow #fff4d1
    };

    grid = std::vector<std::vector<TileInfo>>(boardLength, std::vector<TileInfo>(boardWidth));

    if (screenIndex != -1) {
        window.drawString(
            infoWidth/3 + 25, SPACING,
            "Player " + std::to_string(players[screenIndex].getIndex() + 1)
            + "'s Screen"
        );
    }

    window.fillRectangle(
        boardX - 3, boardY - 3,
        boardDimension + 6, boardDimension + 6,
        window.getColor(4, 4, 4) // Dark grey background/border #040404
    );
}


void GraphicalView::convertCoordinatesPOV(int& row, int& col, int POVindex) {
    if (POVindex == 0) {
        row = grid.size() - row - 1;
        col = grid[0].size() - col - 1;
    } else if (POVindex == 2) {
        std::swap(row, col);
        col = grid[0].size() - col - 1;
    } else if (POVindex == 3) {
        std::swap(row, col);
        row = grid.size() - row - 1;
    }
}

void GraphicalView::drawTile(int x, int y, int width, int height, const TileInfo& tileInfo) {
    long backgroundColor = window.getColor(240, 240, 240); // default empty = light grey #f0f0f0
    long linkColor = 0;
    long linkTypeColor = 0;
    long effectInfoColor = 0;
    std::string text = "";

    if (tileInfo.isTrap) {
        backgroundColor = window.getColor(243, 213, 153); // trap color rgba(243, 213, 153, 1)
        text = "T";
    }
    else if (tileInfo.isWall) {
        backgroundColor = window.getColor(43, 43, 43); // dark grey #2b2b2b
    }
    else if (tileInfo.isGoal) {
        std::vector<int> serverPortColor = serverPortColors[(tileInfo.effectOwner + 1) % 2];
        backgroundColor = window.getColor(serverPortColor[0], serverPortColor[1], serverPortColor[2]);
        text = "$";
    }
    else if (tileInfo.isWater) {
        backgroundColor = window.getColor(13, 219, 222); // light blue #0ddbdeff
        text = "W";
    }

    if (tileInfo.hasTileEffect && !tileInfo.isGoal) {
        std::vector<int> playerColor = playerColors[tileInfo.effectOwner];
        effectInfoColor = window.getColor(playerColor[0], playerColor[1], playerColor[2]);
    }

    if (tileInfo.hasGamePiece) {
        std::vector<int> playerColor = playerColors[tileInfo.pieceOwner];
        linkColor = window.getColor(playerColor[0], playerColor[1], playerColor[2]);   
        text = tileInfo.piece;
    }

    // Background with gaps to have small tile borders
    window.fillRectangle(x+1, y+1, width-2, height-2, backgroundColor);

    if (effectInfoColor) {
        // Tile effect owner info (small bar on bottom)
        window.fillRectangle(x+2, y+height-9, width-4, 6, effectInfoColor);
    }

    if (linkColor) {
        if (tileInfo.hasTileEffect) {
            window.fillRectangle(x+5, y+5, width-10, height-10, linkColor);
        } else {
            window.fillRectangle(x+4, y+4, width-8, height-8, linkColor);
        }
        if (linkTypeColor) {
            // Link virus/data type info (small square top right)
            window.fillRectangle(x+width-15, y+6, 9, 9, linkTypeColor);
        }
    }

    if (!text.empty()) {
        int textX = width / 2 - (text.length() * 3); // center text
        int textY = height / 2 + 5;
        window.drawString(x + textX, y + textY, text);
    }
}

void GraphicalView::drawTile(const Tile& tile) {
    int row = tile.getRow();
    int col = tile.getColumn();

    if (row == 0 || row == grid.size() - 1 || col == 0 || col == grid[0].size() -1) return;
    
    // getting the info need to be before conversion happens
    const TileInfo& tileInfo = grid[row][col];
    
    if (POVEnabled) {
        // convertPOV changes the row and col
        if (screenIndex == -1) {
            // singleScreen
            convertCoordinatesPOV(row, col, currentPlayer);
        } else {
            convertCoordinatesPOV(row, col, screenIndex);
        }
    }
    
    // for spacing alignment
    row--; col--;

    // draw just the updated tile
    drawTile(boardX + col * TILE_SIZE + TILE_SIZE, boardY + row * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileInfo);

    // window.flush();
}

// for single screen
void GraphicalView::drawWholeBoard() {
    for (int row = 1; row < grid.size() -1; row++) {
        for (int col = 1; col < grid[0].size() -1; col++) {

            int r = row;
            int c = col;
            
            // getting the info needs to be before conversion
            const TileInfo& tileInfo = grid[r][c];
            
            if (POVEnabled) {
                // convertPOV changes the r and c
                if (screenIndex == -1) {
                    // singleScreen
                    convertCoordinatesPOV(r, c, currentPlayer);
                } else {
                    convertCoordinatesPOV(r, c, screenIndex);
                }
            }
            
            
            
            // for spacing alignment
            r--; c--;
        
            drawTile(boardX + c * TILE_SIZE + TILE_SIZE, boardY + r * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileInfo);
        }
    }
    window.flush();

}

void GraphicalView::drawPlayerInfo(int x, int y, int width, int height, int playerIndex, bool show) {
    // refresh
    std::vector<int> color = serverPortColors[playerIndex];
    window.fillRectangle(x, y, width, height, window.getColor(color[0], color[1], color[2]));

    int margin = 8;
    int startX = x + margin;
    int startY = y + LINE_HEIGHT;

    // write info
    window.drawString(
        startX, startY,
        "Player " + std::to_string(players[playerIndex].getIndex() + 1)
        + ((playerIndex == currentPlayer) ? ": <- Active Player" : ":")
    );

    // pieces info
    std::string text = "";
    for (auto& piece : players[playerIndex].pieces) {
        std::string pieceText = std::string(0, piece.second->getPiece());
        std::string isDeadText = (piece.second->isDead() ? "Dead" : "Alive");
        text += std::string(1, piece.first) + ": " + pieceText + isDeadText;
        
        text += " ";
    }

    // print it
    window.drawString(startX, startY + 2 * LINE_HEIGHT, text);
}


void GraphicalView::updateGrid(const Tile& tile) {
    // Priority: Wall -> Link -> TileEffect -> Normal
    int row = tile.getRow();
    int col = tile.getColumn();
    GamePiece* piece = tile.getPiece();
    TileEffect* tileEffect = tile.getTileEffect();

    TileInfo& tileInfo = grid[row][col];

    tileInfo.initialized = true;
    tileInfo.isWall = tile.getIsWall();
    tileInfo.isWater = tile.getIsWater();
    tileInfo.hasGamePiece = (piece != nullptr);

    if (piece) {
        tileInfo.piece = piece->getPiece();
        tileInfo.strength = piece->getStrength();
        tileInfo.pieceOwner = piece->getOwner()->getIndex();
    }

    tileInfo.hasTileEffect = (tileEffect != nullptr);

    if (tileEffect) {
        tileInfo.effectOwner = tileEffect->getPlayer()->getIndex();
        
        tileInfo.isTrap = dynamic_cast<TrapEffect*>(tileEffect) != nullptr;
        tileInfo.isGoal = dynamic_cast<GoalEffect*>(tileEffect) != nullptr;
    } else {
        tileInfo.effectOwner = -1;
        
        tileInfo.isTrap = false;
        tileInfo.isGoal = false;
    }
}


void GraphicalView::notify(const Tile& tile) {
    updateGrid(tile);

    if (!displayReady) {
        displayReady = checkIsReady();
    }

    // Draws all info only if ready
    if (displayReady) {
        redrawPlayers();
    }

    // Draws board
    drawTile(tile);

    if (displayReady) {
        window.flush();
    }
}


bool GraphicalView::checkIsReady() {
    for (const auto& row : grid) {
        for (const auto& tileInfo : row) {
            if (!tileInfo.initialized) {
                return false;
            }
        }
    }
    return true;
}


void GraphicalView::print(std::ostream& out, int playerIndex, bool POV) {
    currentPlayer = playerIndex;
}

void GraphicalView::printWhoWon(std::ostream& out, int playerIndex) {
    int margin = 8;
    
    window.drawString(boardX, boardY, "PLAYER " + std::to_string(playerIndex) + " WON THE GAME!!!");
    window.drawString(boardX, boardY + boardDimension + LINE_HEIGHT, "PLAYER " + std::to_string(playerIndex) + " WON THE GAME!!!");
    
    window.fillRectangle(infoX, infoY, infoWidth, infoHeight, window.getColor(237, 237, 221));
    window.drawString(infoX + margin, infoY + LINE_HEIGHT, "PLAYER " + std::to_string(playerIndex) + " WON THE GAME!!!");

}

void GraphicalView::redrawPlayers() {
    // print player info before and after board, and abilities
    for (int i = 0, count = 0; i < players.size(); i += 2, count++) {
        bool show = (screenIndex == -1) ? (i == currentPlayer) : (i == screenIndex);

        drawPlayerInfo(
            playerInfoX,
            playerInfo1Y + count * (playerInfoHeight + SPACING),
            playerInfoWidth,
            playerInfoHeight,
            i,
            show
        );
    }
    
    for (int i = 1, count = 0; i < players.size(); i += 2, count++) {
        bool show = (screenIndex == -1) ? (i == currentPlayer) : (i == screenIndex);

        drawPlayerInfo(
            playerInfoX,
            playerInfo2Y + count * (playerInfoHeight + SPACING),
            playerInfoWidth,
            playerInfoHeight,
            i,
            show
        );
    }
    window.flush();
}
