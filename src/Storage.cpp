#include "Storage.h"
#include "Utils.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <iostream>

void Storage::load(const char* filename, int board[9][9], bool fixed[9][9], Uint32& elapsedSeconds) {
    int sum = 0;
    std::ifstream file(filename);
    if (!file) return;

    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j) {
            int t;
            file >> t;
            sum += t;
        }
    file.close();
    if (sum == 0) {
        Utils::generatePuzzle();
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                board[i][j] = Utils::puzzle[i][j];
                fixed[i][j] = Utils::puzzle[i][j] != 0;
            }
        }
        return;
    }
    file.open(filename);

    for (int i = 0; i < 9; ++i) 
        for (int j = 0; j < 9; ++j) {
            file >> Utils::board[i][j];
        }
    for (int i = 0; i < 9; ++i) 
        for (int j = 0; j < 9; ++j) 
            file >> board[i][j];
    for (int i = 0; i < 9; ++i) 
        for (int j = 0; j < 9; ++j) 
            file >> fixed[i][j];

    std::string timeStr;
    file >> timeStr;  //"mm:ss"
    int minutes = 0, seconds = 0;
    if (sscanf(timeStr.c_str(), "%d:%d", &minutes, &seconds) == 2)
        elapsedSeconds = minutes * 60 + seconds;
    else
        elapsedSeconds = 0;  // nếu lỗi format

    file.close();
}

void Storage::save(const char* filename, int board[9][9], bool fixed[9][9], int elapsedSeconds) {
    std::ofstream file(filename);
    if (!file) return;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            file << Utils::board[i][j] << " ";
        }
        file << "\n";
    }
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            file << board[i][j] << " ";
        }
        file << "\n";
    }
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            file << fixed[i][j] << " ";
        }
        file << "\n";
    }

    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;
    file << (minutes < 10 ? "0" : "") << minutes << ":"
         << (seconds < 10 ? "0" : "") << seconds << "\n";

    file.close();
}
