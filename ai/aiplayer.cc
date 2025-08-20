#include "aiplayer.h"
#include "ainetwork.h"
#include "../game/board.h"
#include "../game/tile.h"
#include "../game/gamepiece.h"
#include "../game/tileeffect.h"
#include "../game/goaleffect.h"
#include "../game/trapeffect.h"
#include <algorithm>
#include <iostream>
#include <cmath>

AIPlayer::AIPlayer(int index, char startingPiece, double learningRate)
    : Player(index, startingPiece), 
      epsilon(1.0),           // Start with high exploration
      epsilonDecay(0.995),    // Gradually reduce exploration
      epsilonMin(0.01),       // Minimum exploration
      learningRate(learningRate),
      memorySize(2000),
      memoryIndex(0),
      rng(std::random_device{}())
{
    // Initialize neural network
    // State size: board positions (11x9) + piece information + game state
    int stateSize = 11 * 9 * 4;  // 4 features per tile: piece, owner, effect, water/wall
    int actionSize = 8 * 4;      // 8 pieces * 4 directions
    
    std::vector<int> networkLayers = {stateSize, 256, 128, 64, actionSize};
    network = std::make_unique<AINetwork>(networkLayers);
    
    memory.reserve(memorySize);
}

AIPlayer::~AIPlayer() = default;

std::vector<float> AIPlayer::boardToStateVector(Board* board) {
    std::vector<float> state;
    state.reserve(11 * 9 * 4);
    
    for (int row = 0; row < board->getLength(); ++row) {
        for (int col = 0; col < board->getWidth(); ++col) {
            Tile* tile = board->getTile(row, col);
            
            // Feature 1: Piece type (normalized)
            GamePiece* piece = tile->getPiece();
            if (piece) {
                state.push_back((piece->getPiece() - '1') / 8.0f);  // Normalize piece ID
            } else {
                state.push_back(0.0f);
            }
            
            // Feature 2: Piece owner (normalized)
            if (piece) {
                state.push_back(piece->getOwner()->getIndex() / 2.0f);  // 0 or 0.5
            } else {
                state.push_back(-1.0f);  // No piece
            }
            
            // Feature 3: Tile effects
            TileEffect* effect = tile->getTileEffect();
            if (effect) {
                if (effect->isGoal()) {
                    state.push_back(1.0f);
                } else if (effect->isTrap()) {
                    state.push_back(0.5f);
                } else {
                    state.push_back(0.25f);
                }
            } else {
                state.push_back(0.0f);
            }
            
            // Feature 4: Terrain (wall/water)
            if (tile->getIsWall()) {
                state.push_back(1.0f);
            } else if (tile->getIsWater()) {
                state.push_back(0.5f);
            } else {
                state.push_back(0.0f);
            }
        }
    }
    
    return state;
}

std::vector<std::pair<char, char>> AIPlayer::getAllValidMoves(Board* board) {
    std::vector<std::pair<char, char>> validMoves;
    std::vector<char> directions = {'N', 'S', 'E', 'W'};
    
    for (const auto& piece : pieces) {
        if (piece.second && !piece.second->isDead()) {
            char pieceId = piece.first;
            
            for (char dir : directions) {
                // Save current position
                int currentRow = piece.second->getRow();
                int currentCol = piece.second->getCol();
                
                // Try the move by calling the player's move method
                // We'll simulate this by checking if the move would be valid
                // For now, add all moves and let the game engine handle validation
                validMoves.push_back({pieceId, dir});
            }
        }
    }
    
    return validMoves;
}

int AIPlayer::actionToIndex(char piece, char direction) {
    int pieceIndex = piece - '1';  // Convert '1'-'8' to 0-7
    int dirIndex;
    switch (direction) {
        case 'N': dirIndex = 0; break;
        case 'S': dirIndex = 1; break;
        case 'E': dirIndex = 2; break;
        case 'W': dirIndex = 3; break;
        default: return -1;
    }
    return pieceIndex * 4 + dirIndex;
}

std::pair<char, char> AIPlayer::indexToAction(int index) {
    int pieceIndex = index / 4;
    int dirIndex = index % 4;
    
    char piece = '1' + pieceIndex;
    char direction;
    switch (dirIndex) {
        case 0: direction = 'N'; break;
        case 1: direction = 'S'; break;
        case 2: direction = 'E'; break;
        case 3: direction = 'W'; break;
        default: direction = 'N';
    }
    
    return {piece, direction};
}

std::pair<char, char> AIPlayer::chooseMove(Board* board) {
    std::vector<std::pair<char, char>> validMoves = getAllValidMoves(board);
    
    if (validMoves.empty()) {
        // No valid moves - this shouldn't happen in a proper game
        return {'1', 'N'};
    }
    
    // Epsilon-greedy action selection
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(rng) < epsilon) {
        // Random action (exploration)
        std::uniform_int_distribution<int> moveChoice(0, validMoves.size() - 1);
        return validMoves[moveChoice(rng)];
    } else {
        // Use neural network (exploitation)
        std::vector<float> state = boardToStateVector(board);
        std::vector<float> qValues = network->predict(state);
        
        // Find the best valid action
        int bestAction = -1;
        float bestQValue = -std::numeric_limits<float>::infinity();
        
        for (const auto& move : validMoves) {
            int actionIndex = actionToIndex(move.first, move.second);
            if (actionIndex >= 0 && actionIndex < qValues.size() && qValues[actionIndex] > bestQValue) {
                bestQValue = qValues[actionIndex];
                bestAction = actionIndex;
            }
        }
        
        if (bestAction >= 0) {
            return indexToAction(bestAction);
        } else {
            // Fallback to random
            std::uniform_int_distribution<int> moveChoice(0, validMoves.size() - 1);
            return validMoves[moveChoice(rng)];
        }
    }
}

float AIPlayer::calculateReward(Constants::MOVE_RESULT result, bool gameWon, bool gameLost) {
    if (gameWon) return 100.0f;
    if (gameLost) return -100.0f;
    
    switch (result) {
        case Constants::MOVE_SUCCESS:
            return 1.0f;
        case Constants::MOVE_KILLED:
            return 10.0f;  // Capturing enemy piece is good
        case Constants::MOVE_INVALID:
        case Constants::MOVE_WALL:
        case Constants::MOVE_OWNPIECE:
        case Constants::MOVE_RAT_INVALID:
        case Constants::MOVE_WATER_INVALID:
            return -5.0f;  // Invalid moves are bad
        default:
            return 0.0f;
    }
}

void AIPlayer::remember(const std::vector<float>& state, int action, float reward,
                       const std::vector<float>& nextState, bool gameOver) {
    Experience exp = {state, action, reward, nextState, gameOver};
    
    if (memory.size() < memorySize) {
        memory.push_back(exp);
    } else {
        memory[memoryIndex] = exp;
        memoryIndex = (memoryIndex + 1) % memorySize;
    }
}

void AIPlayer::updateExperience(const std::vector<float>& state, int action, float reward,
                               const std::vector<float>& nextState, bool gameOver) {
    remember(state, action, reward, nextState, gameOver);
}

void AIPlayer::trainOnBatch() {
    if (memory.size() < 32) return;  // Wait until we have enough experiences
    
    replay(32);
}

void AIPlayer::replay(int batchSize) {
    if (memory.size() < batchSize) return;
    
    // Sample random batch from memory
    std::vector<int> indices(memory.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng);
    
    std::vector<std::vector<float>> states, targets;
    
    for (int i = 0; i < std::min(batchSize, (int)memory.size()); ++i) {
        const Experience& exp = memory[indices[i]];
        
        std::vector<float> target = network->predict(exp.state);
        
        if (exp.gameOver) {
            target[exp.action] = exp.reward;
        } else {
            std::vector<float> nextQValues = network->predict(exp.nextState);
            float maxNextQ = *std::max_element(nextQValues.begin(), nextQValues.end());
            target[exp.action] = exp.reward + 0.95f * maxNextQ;  // Discount factor
        }
        
        states.push_back(exp.state);
        targets.push_back(target);
    }
    
    network->trainBatch(states, targets, learningRate);
}

void AIPlayer::saveModel(const std::string& filename) {
    network->saveToFile(filename);
}

void AIPlayer::loadModel(const std::string& filename) {
    network->loadFromFile(filename);
}
