#pragma once
#include <vector>
#include <random>
#include <algorithm>

class Utils {
private:
    static bool isCellValid(int row, int col, int value);
    static bool solve(int pos = 0);
    static void transpose();
    static void swapRowsInBlock(int block);
    static void swapColsInBlock(int block);
    static void permuteNumbers();
    static void generateBoard();
    static void removeNumbers(int b[9][9], int count);

public:
    Utils() = delete; 
    static int board[9][9];
    static int puzzle[9][9];
    static std::mt19937 rng;

    static bool isPuzzleFull();
    static bool checkWin();
    static void generatePuzzle();
    static int getHint(int row, int col);
    static std::vector<std::pair<int, int>> checkWrongCells(int playerBoard[9][9]);
};
