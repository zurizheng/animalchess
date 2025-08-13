#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__


namespace Constants {
    constexpr int BOARD_SIZE_2_PLAYER = 10;
    constexpr int BOARD_SIZE_4_PLAYER = 12;
    constexpr const char* BOARD_2_PLAYER = "2player.txt";
    constexpr const char* BOARD_4_PLAYER = "4player.txt";

    // Starting characters for each of 4 players
    // all characters for the players are starting + next 7 chars
    constexpr char PLAYER_STARTING_PIECES[4] = {'a', 'A', 'n', 'N'};
    constexpr int NUM_LINKS = 8;
    constexpr int NUM_ABILITIES = 5;

    enum MOVE_RESULT {
        MOVE_SUCCESS,
        MOVE_INVALID,
        MOVE_WALL,
        MOVE_OWNLINK,
        MOVE_DOWNLOADED
    };
}

#endif // __CONSTANTS_H__
