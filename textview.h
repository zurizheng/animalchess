#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

#include <vector>
#include "view.h"

class TextView : public View {
    std::vector<std::vector<char>> grid;
    
    void printPlayerInfo(std::ostream& out, int playerIndex, bool show);
    void convertCoordinatesPOV(int& row, int& col, int POVindex);

    public:
        TextView(int length, const std::vector<Player>& players);
        void notify(const Tile& tile) override;
        void print(std::ostream& out, int playerIndex, bool POV) override;
        void printWhoWon(std::ostream& out, int playerIndex) override;
        void printGrid(std::ostream& out, int playerIndex, bool POV);
        void printStartTurn(std::ostream& out, int playerIndex) override;
        void printEndTurn(std::ostream& out) override;
};

#endif



