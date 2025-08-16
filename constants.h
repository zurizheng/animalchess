#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__


namespace Constants {
    constexpr int BOARD_SIZE_2_PLAYER = 11;
    constexpr int BOARD_WIDTH_2_PLAYER = 9;
    constexpr const char* BOARD_2_PLAYER = "board.txt";

    // all characters for the players are starting + next 7 chars
    constexpr char PLAYER_STARTING_PIECES[2] = {'1', '1'};
    constexpr int NUM_PIECES = 8;
    constexpr int NUM_ABILITIES = 5;

    enum MOVE_RESULT {
        MOVE_SUCCESS,
        MOVE_INVALID,
        MOVE_WALL,
        MOVE_OWNPIECE,
        MOVE_KILLED
    };
}

#endif // __CONSTANTS_H__
