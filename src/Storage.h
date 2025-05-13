#pragma once
#include <SDL.h>

class Storage {
public:
    static void load(const char* filename, int board[9][9], bool fixed[9][9], Uint32& elapsedSeconds);
    static void save(const char* filename, int board[9][9], bool fixed[9][9], int elapsedSeconds);
};
