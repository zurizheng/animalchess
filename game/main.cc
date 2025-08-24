#include "controller.h"

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

int main(int argc, char * argv[]) {
    std::string command;

    int numPlayers = 2; // Default to 2 players;
    bool useGraphics = false;
    bool viewPerPlayer = false;
    bool POVEnabled = false;
    bool aiOpponent = false;

    for (int i = 1; i < argc; i++) {
        command = argv[i];

        if (command == "-graphics") {
            useGraphics = true;
        }

        if (command == "-pov") {
            POVEnabled = true;
        }

        if (command == "-splitview") {
            viewPerPlayer = true;
        }

        if (command == "-ai") {
            aiOpponent = true;
        }

        if (command == "-help") {
            std::cout << "Usage: " << argv[0] << " [-graphics] [-pov] [-splitview] [-ai]" << std::endl;
            std::cout << "  -graphics    Enable graphical interface" << std::endl;
            std::cout << "  -pov         Enable point-of-view mode" << std::endl;
            std::cout << "  -splitview   Enable split view for multiple players" << std::endl;
            std::cout << "  -ai          Play against AI (requires trained model)" << std::endl;
            return 0;
        }
    }

    Controller controller(
        numPlayers,
        useGraphics,
        viewPerPlayer,
        POVEnabled,
        false  // Not in training mode
    );

    if (aiOpponent) {
        // Set player 1 as AI (better performing player from training)
        controller.setAIPlayer(0, 0.001);
        std::cout << "Playing against AI! You are Player 2." << std::endl;
    }

    controller.play();

    return 0;
}

