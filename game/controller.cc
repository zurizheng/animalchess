#include "controller.h"

#include "board.h"
#include "constants.h"
#include "player.h"
#include "../ai/aiplayer.h"
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
    bool POVEnabled,
    bool aiTraining
) : currentPlayer{0}, useGraphics{useGraphics}, viewPerPlayer{viewPerPlayer}, POVEnabled{POVEnabled}, aiTraining{aiTraining}
{
    // Set up initial input stack (default to cin)
    inputStack.push(std::make_unique<std::istream>(std::cin.rdbuf()));

    // Initialize players and their abilities
    players.reserve(numPlayers);
    for (int i = 0; i < numPlayers; i++) {
        // Creates links map for each player with nullptr link - gets set later
        players.emplace_back(i, Constants::PLAYER_STARTING_PIECES[i]);
    }

    int boardLength = Constants::BOARD_SIZE_2_PLAYER;
    int boardWidth = Constants::BOARD_WIDTH_2_PLAYER;


    // Initialize text and optionally graphical views
    tv = std::make_unique<TextView>(boardLength, boardWidth, players);
    if (useGraphics) {
        if (viewPerPlayer) {
            for (int i = 0; i < numPlayers; i++) {
                graphicalViews.push_back(
                    std::make_unique<GraphicalView>(boardLength, boardWidth, i, players, POVEnabled)
                );
            }
        }
        else {
            graphicalViews.push_back(
                std::make_unique<GraphicalView>(boardLength, boardWidth, -1, players, POVEnabled)
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
    board = std::make_unique<Board>(boardLength, boardWidth, this);
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

        // Check if current player is AI
        if (isAIPlayer(currentPlayer)) {
            handleAITurn();
            if (!nextTurn()) break;
            continue;
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
            else if (result == Constants::MOVE_RAT_INVALID) {
                std::cout << "rat cannot move into land/water and battle" << std::endl;
            } else if (result == Constants::MOVE_WATER_INVALID) {
                std::cout << "invalid water move" << std::endl;
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
        row = board->getLength() - row - 1;
        col = board->getWidth() - col - 1;
    } else if (POVindex == 2) {
        std::swap(row, col);
        row = board->getLength() - row - 1;
    } else if (POVindex == 3) {
        std::swap(row, col);
        col = board->getWidth() - col - 1;
    }
}

void Controller::setAIPlayer(int playerIndex, double learningRate) {
    if (playerIndex >= 0 && playerIndex < players.size()) {
        // Ensure we have enough AI player slots
        while (aiPlayers.size() <= playerIndex) {
            aiPlayers.push_back(nullptr);
        }
        
        // Create AI player with same starting piece as regular player
        char startingPiece = Constants::PLAYER_STARTING_PIECES[playerIndex];
        aiPlayers[playerIndex] = std::make_unique<AIPlayer>(playerIndex, startingPiece, learningRate);
        
        std::cout << "Player " << (playerIndex + 1) << " set as AI" << std::endl;
    }
}

bool Controller::isAIPlayer(int playerIndex) const {
    return playerIndex < aiPlayers.size() && aiPlayers[playerIndex] != nullptr;
}

bool Controller::handleAITurn() {
    if (!isAIPlayer(currentPlayer)) {
        return false;  // Not an AI player
    }
    
    // Get AI move
    auto move = aiPlayers[currentPlayer]->chooseMove(board.get());
    char pieceId = move.first;
    char direction = move.second;
    
    // Execute the move
    Constants::MOVE_RESULT result = players[currentPlayer].move(board.get(), pieceId, direction);
    
    // Calculate reward for AI learning
    bool gameWon = players[currentPlayer].getHasWon();
    bool gameLost = gameOver() && !gameWon;
    
    if (aiTraining) {
        // Store experience for learning
        std::vector<float> currentState = aiPlayers[currentPlayer]->boardToStateVector(board.get());
        int actionIndex = aiPlayers[currentPlayer]->actionToIndex(pieceId, direction);
        float reward = aiPlayers[currentPlayer]->calculateReward(result, gameWon, gameLost);
        
        // For training, we'll update the experience after the next move
        // This is a simplified approach - in a full implementation you'd store the experience
        // and update it with the next state after the opponent's move
    }
    
    if (result == Constants::MOVE_SUCCESS || result == Constants::MOVE_KILLED) {
        if (!aiTraining) {
            tv->print(std::cout, currentPlayer, POVEnabled);
            std::cout << "AI Player " << (currentPlayer + 1) << " moved piece " 
                      << pieceId << " " << direction << std::endl;
        }
        return true;  // Valid move made
    } else {
        if (!aiTraining) {
            std::cout << "AI Player " << (currentPlayer + 1) << " made invalid move: " 
                      << pieceId << " " << direction << std::endl;
        }
        return true;  // Still handled, even if invalid
    }
}

void Controller::trainAI(int numGames) {
    if (!aiTraining) {
        std::cout << "Controller not in training mode!" << std::endl;
        return;
    }
    
    for (int game = 0; game < numGames; ++game) {
        // Reset the game
        // Note: You'll need to implement a reset method for the board
        currentPlayer = 0;
        
        int moves = 0;
        while (!gameOver() && moves < 200) {  // Limit moves to prevent infinite games
            if (isAIPlayer(currentPlayer)) {
                handleAITurn();
            }
            
            if (!nextTurn()) break;
            moves++;
        }
        
        // Train the AI players after each game
        for (auto& aiPlayer : aiPlayers) {
            if (aiPlayer) {
                aiPlayer->trainOnBatch();
                aiPlayer->decayEpsilon();  // Reduce exploration over time
            }
        }
        
        if (game % 100 == 0) {
            std::cout << "Completed " << game << " training games..." << std::endl;
            
            // Save progress
            for (int i = 0; i < aiPlayers.size(); ++i) {
                if (aiPlayers[i]) {
                    std::string filename = "ai_player_" + std::to_string(i) + ".model";
                    aiPlayers[i]->saveModel(filename);
                }
            }
        }
    }
    
    // Final save
    for (int i = 0; i < aiPlayers.size(); ++i) {
        if (aiPlayers[i]) {
            std::string filename = "ai_player_" + std::to_string(i) + "_final.model";
            aiPlayers[i]->saveModel(filename);
        }
    }
}

void Controller::playAgainstAI() {
    // Load trained AI model
    if (aiPlayers.size() > 1 && aiPlayers[1]) {
        aiPlayers[1]->loadModel("ai_player_1_final.model");
        aiPlayers[1]->setEpsilon(0.05);  // Low exploration for playing
    }
    
    // Normal game loop with AI handling
    play();
}
