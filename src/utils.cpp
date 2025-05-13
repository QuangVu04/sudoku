#include "Utils.h"

int Utils::board[9][9] = {0};
int Utils::puzzle[9][9] = {0};

std::mt19937 Utils::rng(std::random_device{}());

bool Utils::isCellValid(int row, int col, int value) {
    for (int j = 0; j < 9; ++j)
        if (board[row][j] == value)
            return false;
    for (int i = 0; i < 9; ++i)
        if (board[i][col] == value)
            return false;

    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[startRow + i][startCol + j] == value)
                return false;

    return true;
}

bool Utils::solve(int pos) {
    if (pos == 81)
        return true;
    int row = pos / 9;
    int col = pos % 9;

    if (board[row][col] != 0)
        return solve(pos + 1);

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle(numbers.begin(), numbers.end(), rng);

    for (int num : numbers)
    {
        if (isCellValid(row, col, num))
        {
            board[row][col] = num;
            if (solve(pos + 1))
                return true;
            board[row][col] = 0;
        }
    }

    return false;
}

void Utils::generateBoard() {
    memset(board, 0, sizeof(board));
    solve();

    for (int i = 0; i < 3; ++i)
    {
        swapRowsInBlock(i);
        swapColsInBlock(i);
    }

    if (rand() % 2)
        transpose();
    permuteNumbers();
}

void Utils::generatePuzzle() {
    generateBoard();
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            puzzle[i][j] = board[i][j];
    int cellsToRemove = 81 - (rand() % (35 - 30 + 1) + 30);  
    removeNumbers(puzzle, cellsToRemove);
}

void Utils::removeNumbers(int b[9][9], int count) {
    int removed = 0;
    while (removed < count) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (b[row][col] != 0) {
            b[row][col] = 0;
            removed++;
        }
    }
}

std::vector<std::pair<int, int>> Utils::checkWrongCells(int playerBoard[9][9]) {
    std::vector<std::pair<int, int>> wrongCells;
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (playerBoard[i][j] != 0 && playerBoard[i][j] != board[i][j])
                wrongCells.push_back({i, j});
    return wrongCells;
}

void Utils::transpose() {
    for (int i = 0; i < 9; ++i)
        for (int j = i + 1; j < 9; ++j)
            std::swap(board[i][j], board[j][i]);
}

void Utils::swapRowsInBlock(int block) {
    int start = block * 3;
    int r1 = start + rand() % 3;
    int r2 = start + rand() % 3;
    while (r1 == r2)
        r2 = start + rand() % 3;
    std::swap(board[r1], board[r2]);
}

void Utils::swapColsInBlock(int block) {
    int start = block * 3;
    int c1 = start + rand() % 3;
    int c2 = start + rand() % 3;
    while (c1 == c2)
        c2 = start + rand() % 3;
    for (int i = 0; i < 9; ++i)
        std::swap(board[i][c1], board[i][c2]);
}

void Utils::permuteNumbers() {
    std::vector<int> mapping(10);
    std::vector<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle(digits.begin(), digits.end(), rng);
    for (int i = 1; i <= 9; ++i)
        mapping[i] = digits[i - 1];

    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            board[i][j] = mapping[board[i][j]];
}
bool Utils::isPuzzleFull() {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (Utils::puzzle[i][j] == 0) return false;
    return true;
}

bool Utils::checkWin() {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (Utils::puzzle[i][j] != Utils::board[i][j])
                return false;
    return true;
}
