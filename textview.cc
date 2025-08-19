#include "textview.h"
#include "player.h"
#include "tile.h"
#include "goaleffect.h"
#include "trapeffect.h"
#include <iostream>
#include <memory>
#include <vector>


TextView::TextView(int length, int width, const std::vector<Player>& players) : View{players} {
    grid = std::vector<std::vector<char>>(length, std::vector<char>(width));
}


void TextView::notify(const Tile& tile) {
    int row = tile.getRow();
    int col = tile.getColumn();
    GamePiece* piece = tile.getPiece();
    TileEffect* tileEffect = tile.getTileEffect();

    // Priority: Wall -> Link -> TileEffect -> Normal

    if (tile.getIsWall()) {
        grid[row][col] = 'X';
    }
    else if (piece) {
        grid[row][col] = piece->getPiece();
    }
    else if (tile.getIsWater()) {
        grid[row][col] = 'W';
    }
    else if (tileEffect) {
        // type checking
        if (auto* g = dynamic_cast<GoalEffect*>(tileEffect)) {
            grid[row][col] = 'G';
        }
        else if (auto* t = dynamic_cast<TrapEffect*>(tileEffect)) {
            grid[row][col] = 'T';
        }
        else {
            grid[row][col] = '&'; // Endpoint tiles
        }
    }
    else {
        grid[row][col] = '.';
    }
}


void TextView::printPlayerInfo(std::ostream& out, int playerIndex, bool show) {
    int inc = 0;
    out << "Player " << players[playerIndex].getIndex() + 1 << (show ? ": <- Active Player" : ":") << std::endl;
    for (auto& piece : players[playerIndex].pieces) {
        out << piece.first << ": " << (piece.second->isDead() ? "Dead" : "Alive");
        // newline after half of the entries
        if (++inc == players[playerIndex].pieces.size()/2) {
            out << std::endl;
        } else {
            out << " ";
        }
    }
    out << std::endl;
}

void TextView::print(std::ostream& out, int playerIndex, bool POV) {
    // Info at top (1 player at top, 2 player at top for 4 player)
    for (int i = 0; i < players.size(); i += 2) {
        printPlayerInfo(out, i, i == playerIndex);
    }

    for (int i = 0; i < grid[0].size(); i++) {
        out << "=";
    }
    out << std::endl;
    
    // The grid
    printGrid(out, playerIndex, POV);

    for (int i = 0; i < grid[0].size(); i++) {
        out << "=";
    }
    out << std::endl;
    
    // Info at top (1 player at top, 2 player at top for 4 player)
    for (int i = 1; i < players.size(); i += 2) {
        printPlayerInfo(out, i, i == playerIndex);
    }
    out << std::endl; // empty line after full board print
}

void TextView::printGrid(std::ostream& out, int playerIndex, bool POV) {
    for (int row = 1; row < grid.size()-1; row++) {
        out << "|";
        for (int col = 1; col < grid[0].size()-1; col++) {
            int tempRow = row;
            int tempCol = col;
            if (POV) {
                convertCoordinatesPOV(tempRow, tempCol, playerIndex);
            }
            out << grid[tempRow][tempCol];
        }
        out << "|";
        out << '\n';
    }
}

void TextView::convertCoordinatesPOV(int& row, int& col, int POVindex) {
    if (POVindex == 0) {
        row = grid.size() - row - 1;
        col = grid.size() - col - 1;
    } else if (POVindex == 2) {
        std::swap(row, col);
        row = grid.size() - row - 1;
    } else if (POVindex == 3) {
        std::swap(row, col);
        col = grid.size() - col - 1;
    }
}

void TextView::printWhoWon(std::ostream& out, int playerIndex) {
    out << "PLAYER " << playerIndex + 1 << " HAS WON THE GAME!!!" << std::endl;
}

void TextView::printStartTurn(std::ostream& out, int playerIndex) {
    out << "======( PLAYER " << playerIndex + 1 << "'s TURN )==========================" << std::endl;
}

void TextView::printEndTurn(std::ostream& out) {
    out << "===================================================\n" << std::endl;
}
