#include "controller.h"

#include "board.h"
#include "constants.h"
#include "player.h"
#include "view.h"
#include "graphicalview.h"
#include "textview.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <utility>


Controller::Controller(
    int numPlayers,
    bool useGraphics,
    bool viewPerPlayer,
    bool POVEnabled
) : currentPlayer{0}, useGraphics{useGraphics}, viewPerPlayer{viewPerPlayer}, POVEnabled{POVEnabled}
{
    // Set up initial input stack (default to cin)
    inputStack.push(std::make_unique<std::istream>(std::cin.rdbuf()));

    // Initialize players and their abilities
    players.reserve(numPlayers);
    for (int i = 0; i < numPlayers; i++) {
        // Creates links map for each player with nullptr link - gets set later
        players.emplace_back(i, Constants::PLAYER_STARTING_PIECES[i]);
    }

    int boardSize = Constants::BOARD_SIZE_2_PLAYER;
    int boardWidth = Constants::BOARD_WIDTH_2_PLAYER;


    // Initialize text and optionally graphical views
    tv = std::make_unique<TextView>(boardSize, boardWidth, players);
    if (useGraphics) {
        if (viewPerPlayer) {
            for (int i = 0; i < numPlayers; i++) {
                graphicalViews.push_back(
                    std::make_unique<GraphicalView>(boardSize, i, players, POVEnabled)
                );
            }
        }
        else {
            graphicalViews.push_back(
                std::make_unique<GraphicalView>(boardSize, -1, players, POVEnabled)
            );
        }
    }

    // Read board layout from file into vector of strings
    std::string boardLayoutFile = Constants::BOARD_2_PLAYER;

    std::ifstream layoutFile{boardLayoutFile};
    std::vector<std::string> layout;
    std::string line;
    while (std::getline(layoutFile, line)) {
        layout.push_back(line);
    }

    // Initialize the board with specified layout, makes links and tiles
    board = std::make_unique<Board>(boardSize, this);
    bool initSuccess = board->init(layout, players);

    // Board must be initialized successfully
    assert(initSuccess);

    // Initial display
    tv->printStartTurn(std::cout, currentPlayer);
    tv->print(std::cout, 0, POVEnabled);

    for (int i = 0; i < graphicalViews.size(); i++) {
        if (!graphicalViews[i]) {
            std::cout << "DEBUG: Shouldn't Happen (#459848722339)" << std::endl;
            continue;
        }

        graphicalViews[i]->print(
            std::cout, currentPlayer
        );
    }
}

bool Controller::nextTurn() {
    if (tv) {
        tv->printEndTurn(std::cout);
    }

    if (gameOver()) {
        return false;
    }

    currentPlayer = (currentPlayer + 1) % players.size();

    if (tv) {
        tv->printStartTurn(std::cout, currentPlayer);
        tv->print(std::cout, currentPlayer, POVEnabled);
    }

    for (int i = 0; i < graphicalViews.size(); i++) {
        if (graphicalViews[i]) {
            graphicalViews[i]->print(std::cout, currentPlayer);
        }
    }

    // update the graphicalview
    for (int i = 0; i < graphicalViews.size(); i++) {
        if (graphicalViews[i]) {
            graphicalViews[i]->redrawPlayers();
            graphicalViews[i]->drawWholeBoard();
        }
    }

    return true;
}


void Controller::play() {
    while (!inputStack.empty()) {
        if (gameOver()) {
            break;
        }

        // Input prompt for current player
        std::cout << "(P" << currentPlayer + 1 << ") > ";

        std::istream& input = *inputStack.top();
        std::string line;

        // No more input in current stream, pop and go to next one
        if (!std::getline(input, line)) {
            inputStack.pop();
            continue;
        }
        
        std::stringstream lineStream{line};
        std::string cmd;
        lineStream >> cmd;

        if (cmd == "quit") {
            std::cout << "[WARNING] Quitting game." << std::endl;
            break;
        }

        if (cmd == "sequence") {
            std::string fileName;
            lineStream >> fileName;

            std::unique_ptr<std::istream> inputFile = std::make_unique<std::ifstream>(fileName);
            if (!inputFile->good()) {
                std::cout << "[WARNING] Could not read or open file: " << fileName << std::endl;
                continue;
            }

            // Add file to top of input stack
            inputStack.push(std::move(inputFile));
            continue;
        }

        else if (cmd == "board") {
            if (tv) {
                tv->print(std::cout, currentPlayer, POVEnabled);
            }
        }

        else if (cmd == "move") {
            char pieceName;
            std::string direction;
            lineStream >> pieceName >> direction;

            char cardinalDir;
            if (direction == "up" || direction == "n" || direction == "N") {
                cardinalDir = 'N';
            }
            else if (direction == "down" || direction == "s" || direction == "S") {
                cardinalDir = 'S';
            }
            else if (direction == "left" || direction == "w" || direction == "W") {
                cardinalDir = 'W';
            }
            else if (direction == "right" || direction == "e" || direction == "E") {
                cardinalDir = 'E';
            }
            else {
                std::cout << "[INVALID] Invalid direction: '" << direction << "'" << std::endl;
                continue;
            }

            if (POVEnabled) {
                // Corresponds to N, S, W, E from each player's perspective
                const char directionMap[4][4] = {
                    {'S', 'N', 'E', 'W'}, // Player 0
                    {'N', 'S', 'W', 'E'}, // Player 1 (default)
                    {'W', 'E', 'S', 'N'}, // Player 2
                    {'E', 'W', 'N', 'S'}  // Player 3
                };

                int directionIndex;
                switch (cardinalDir) {
                    case 'N': directionIndex = 0; break;
                    case 'S': directionIndex = 1; break;
                    case 'W': directionIndex = 2; break;
                    case 'E': directionIndex = 3; break;
                    default: break; // Should not happen
                }

                cardinalDir = directionMap[currentPlayer][directionIndex];
            }

            // Check that player has the link
            if (players[currentPlayer].pieces.find(pieceName) == players[currentPlayer].pieces.end()) {
                std::cout << "[INVALID] Could not find link '" << pieceName 
                          << "' for player " << currentPlayer + 1 << std::endl;
                continue;
            }

            Constants::MOVE_RESULT result = players[currentPlayer].move(board.get(), pieceName, cardinalDir);

            if (result == Constants::MOVE_SUCCESS || result == Constants::MOVE_KILLED) {
                tv->print(std::cout, currentPlayer, POVEnabled);
                bool result = nextTurn();
                if (!result) return;
                continue;
            }

            std::cout << "[INVALID] Cannot move link '" << pieceName << "' " << direction << ": ";
            if (result == Constants::MOVE_INVALID) {
                std::cout << "invalid input (link may be dead)" << std::endl;
            }
            else if (result == Constants::MOVE_WALL) {
                std::cout << "cannot move into a wall" << std::endl;
            }
            else if (result == Constants::MOVE_OWNPIECE) {
                std::cout << "cannot move into own piece" << std::endl;
            }
            continue;
        }

    }
}

bool Controller::gameOver() {
    for (int i = 0; i < players.size(); i++) {
        if (players[i].getHasWon()) {
            announceWinner(players[i].getIndex());
            return true;
        }
    }
    return false;
}


void Controller::announceWinner(int playerIndex) {
    if (tv) {
        tv->printWhoWon(std::cout, playerIndex);
    }

    for (int i = 0; i < graphicalViews.size(); i++) {
        if (graphicalViews[i]) {
            graphicalViews[i]->printWhoWon(std::cout, playerIndex);
        }
    }
}

void Controller::notify(const Tile& tile) {
    if (tv) {
        tv->notify(tile);
    }
    
    for (int i = 0; i < graphicalViews.size(); i++) {
        if (graphicalViews[i]) {
            graphicalViews[i]->notify(tile);
        }
    }
}


void Controller::convertCoordinatesPOV(int& row, int& col, int POVindex) {
    if (POVindex == 0) {
        row = board->getSize() - row - 1;
        col = board->getSize() - col - 1;
    } else if (POVindex == 2) {
        std::swap(row, col);
        row = board->getSize() - row - 1;
    } else if (POVindex == 3) {
        std::swap(row, col);
        col = board->getSize() - col - 1;
    }
}

