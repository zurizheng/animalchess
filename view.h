#ifndef __VIEW_H__
#define __VIEW_H__

#include <iostream>
#include <vector>

class Tile;
class Player;

class View {
    protected:
        const std::vector<Player>& players;
    public:
        View(const std::vector<Player>& players);
        virtual ~View() = 0;
        virtual void print(std::ostream& out, int playerIndex, bool POV);
        virtual void printWhoWon(std::ostream& out, int playerIndex);
        virtual void printStartTurn(std::ostream& out, int playerIndex);
        virtual void printEndTurn(std::ostream& out);
        virtual void notify(const Tile& tile) = 0;
};

#endif
