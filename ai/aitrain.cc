#include "../game/controller.h"
#include "aiplayer.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    std::cout << "Animal Chess AI Training" << std::endl;
    std::cout << "========================" << std::endl;
    
    // Parse command line arguments
    int numGames = 1000;
    bool graphics = false;
    bool visualize = true;  // Enable visualization by default
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-games" && i + 1 < argc) {
            numGames = std::stoi(argv[i + 1]);
            i++;
        } else if (arg == "-graphics") {
            graphics = true;
        } else if (arg == "-novis") {
            visualize = false;
        } else if (arg == "-help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -games N     Number of training games (default: 1000)" << std::endl;
            std::cout << "  -graphics    Enable graphics during training" << std::endl;
            std::cout << "  -novis       Disable training visualization" << std::endl;
            std::cout << "  -help        Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Create controller for AI training
    Controller controller(2, graphics, false, false, true);  // 2 players, training mode
    
    // Set both players as AI
    controller.setAIPlayer(0, 0.005);  // Player 1 as AI with higher learning rate
    controller.setAIPlayer(1, 0.005);  // Player 2 as AI with higher learning rate
    
    std::cout << "Starting AI training with " << numGames << " games..." << std::endl;
    if (visualize) {
        std::cout << "Training visualization enabled" << std::endl;
        std::cout << "Graphs will show progress every 100 games" << std::endl;
        std::cout << "Final CSV data will be saved to training_data.csv" << std::endl;
    }
    
    // Train the AI
    controller.trainAI(numGames);
    
    std::cout << "Training completed!" << std::endl;
    std::cout << "You can now play against the AI using the main game with -ai flag" << std::endl;
    
    return 0;
}
