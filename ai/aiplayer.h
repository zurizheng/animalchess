#ifndef __AIPLAYER_H__
#define __AIPLAYER_H__

#include "../game/player.h"
#include "../game/board.h"
#include "../game/constants.h"
#include <vector>
#include <random>
#include <memory>

// Forward declarations
class GameState;
class AINetwork;

class AIPlayer : public Player {
private:
    std::unique_ptr<AINetwork> network;
    std::mt19937 rng;
    
    // AI parameters
    double epsilon;          // Exploration rate
    double epsilonDecay;     // How much epsilon decreases per game
    double epsilonMin;       // Minimum epsilon value
    double learningRate;
    int memorySize;
    
    // Experience replay memory
    struct Experience {
        std::vector<float> state;
        int action;
        float reward;
        std::vector<float> nextState;
        bool gameOver;
    };
    
    std::vector<Experience> memory;
    int memoryIndex;
    
    // Reward tracking for visualization
    double totalReward;
    std::vector<double> gameRewards;
    
    // Training mode flag (suppress debug output during training)
    bool trainingMode;
    
    // Helper methods (private)
    std::vector<std::pair<char, char>> getAllValidMoves(Board* board);
    std::pair<char, char> indexToAction(int index);
    
    void remember(const std::vector<float>& state, int action, float reward, 
                 const std::vector<float>& nextState, bool gameOver);
    void replay(int batchSize);
    
public:
    AIPlayer(int index, char startingPiece, double learningRate = 0.001);
    ~AIPlayer();
    
    // Public methods so Controller can access them
    std::vector<float> boardToStateVector(Board* board);
    int actionToIndex(char piece, char direction);
    float calculateReward(Constants::MOVE_RESULT result, bool gameWon, bool gameLost, Board* board = nullptr, char pieceId = '0');
    float calculateGoalProgressReward(Board* board, char pieceId);
    
    // Override the move method to use AI decision making
    std::pair<char, char> chooseMove(Board* board);
    
    // Training methods
    void updateExperience(const std::vector<float>& state, int action, float reward,
                         const std::vector<float>& nextState, bool gameOver);
    void trainOnBatch();
    
    // Save/load the neural network
    void saveModel(const std::string& filename);
    void loadModel(const std::string& filename);
    
    // Getters/setters for AI parameters
    void setEpsilon(double eps) { epsilon = eps; }
    double getEpsilon() const { return epsilon; }
    void decayEpsilon() { 
        if (epsilon > epsilonMin) {
            epsilon *= epsilonDecay;
        }
    }
    
    // Training mode control
    void setTrainingMode(bool training) { trainingMode = training; }
    bool isTrainingMode() const { return trainingMode; }
    
    // Reward tracking methods
    void addReward(double reward) { totalReward += reward; }
    void resetGameReward() { totalReward = 0.0; }
    void recordGameReward() { gameRewards.push_back(totalReward); }
    double getLastGameReward() const { 
        return gameRewards.empty() ? 0.0 : gameRewards.back(); 
    }
    const std::vector<double>& getAllGameRewards() const { return gameRewards; }
};

#endif
