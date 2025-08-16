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

        if (command == "-help") {
            std::cout << "Usage: " << argv[0] << " [-graphics] [-pov] [-splitview]" << std::endl;
            return 0;
        }
    }

    Controller controller(
        numPlayers,
        useGraphics,
        viewPerPlayer,
        POVEnabled
    );

    controller.play();

    return 0;
}

