#include "view.h"
#include "player.h"

View::View(const std::vector<Player>& players) : players(players) {}

View::~View() = default;

void View::print(std::ostream& out, int playerIndex, bool POV) {
    // Do nothing...
}

void View::printWhoWon(std::ostream& out, int playerIndex) {
    // YIPEE!! THERE IS STUFF!!!
}

void View::printStartTurn(std::ostream& out, int playerIndex) {
    // ABCDEFG
}

void View::printEndTurn(std::ostream& out) {
    // HIJKLMNOP
}

