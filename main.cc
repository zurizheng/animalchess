#include "controller.h"

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>


std::vector<std::pair<bool, int>> getPlacementsFromFile(std::string filename) {
    // File contents should be of form eg. "V1 D4 V3 V2 D3 V4 D2 D1"
    std::vector<std::pair<bool, int>> placements;

    std::ifstream file{filename};

    std::string assignment;
    while (file >> assignment) {
        bool isVirus = (assignment[0] == 'V');
        int strength = assignment[1] - '0'; // Convert char to int
        placements.push_back(std::make_pair(isVirus, strength));
    }

    return placements;
}


int main(int argc, char * argv[]) {
    std::string command;

    std::vector<std::string> playerAbilities{4};
    std::vector<std::vector<std::pair<bool, int>>> playerPlacements{4};

    int numPlayers = 2; // Default to 2 players;
    bool useGraphics = false;
    bool viewPerPlayer = false;
    bool POVEnabled = false;

    bool setupCLI = (argc == 1);

    for (int i = 1; i < argc; i++) {
        command = argv[i];

        // Setup command uses command line interface in controller to set up rather than CLI
        if (command == "-setup") {
            setupCLI = true;
            break;
        }

        if (command.rfind("-4player", 0) == 0) {
            numPlayers = 4;
        }

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
            std::cout << "Usage: " << argv[0] << " [-4player] [-ability<n> <abilities>] [-link<n> <placement file>] [-graphics] [-pov] [-splitview]" << std::endl;
            std::cout << "Abilities can be: L (LinkBoost), F (Firewall), D (Download), S (Scan), P (Polarize), W (Swap), H (PhaseShift), ~ (Slimify)" << std::endl;
            return 0;
        }

        // TODO misformed CLA/error handling?
        if (command.rfind("-ability", 0) == 0) {
            int n = std::stoi(command.substr(8)) - 1;

            // Next argument should be the string of abilities themselves
            std::string abilities = argv[i+1];
            playerAbilities[n] = abilities;
            
            // Skip the next argument (because it's the abilities string we just read)
            i++; 
        }

        if (command.rfind("-link", 0) == 0) {
            int n = std::stoi(command.substr(5)) - 1;

            // Read placements from placement file
            std::string placementFile = argv[i+1];
            playerPlacements[n] = getPlacementsFromFile(placementFile);

            // Skip the next argument (because it's the placement filename we just read)
            i++;
        }
    }

    Controller controller(
        numPlayers,
        playerPlacements,
        useGraphics,
        viewPerPlayer,
        POVEnabled,
        setupCLI
    );

    controller.play();

    return 0;
}

