#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Timer.h"
#include "Button.h"
#include <SDL_mixer.h>

enum GameState {
    STATE_MENU,
    STATE_LEVEL,
    STATE_PLAYING,
    STATE_EXIT
};

class Board {
public:
    Board(SDL_Renderer* renderer, SDL_Texture* muteTexture, Timer* timer, int windowWidth, int windowHeight);
    ~Board();

    void render();
    void handleEvent(SDL_Event& e);

private:
    const int PADDING_TOP = 50;
    const int CELL_SIZE = 50;
    const int BOARD_SIZE = 9 * CELL_SIZE + 18;

    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* muteTexture;
    int selectedRow, selectedCol, selectedValue;
    int hoverRow, hoverCol;
    int board[9][9];
    bool fixed[9][9];
    Timer* timer;
    bool showVictoryMessage;
    Mix_Chunk* victorySound;
    bool bgmMuted = false;
    GameState currentState = STATE_MENU;

    int paddingLeft, paddingTop;
    int difficultyLevel = 30;
    std::vector<Button*> buttons;
    std::vector<Button*> menuButtons;
    std::vector<Button*> levelButtons;
    std::vector<std::pair<int, int>> wrongCells;

    void drawGrid();
    void drawNumbers();
    void loadFromFile();
    void saveToFile();
    int getOffsetWithThickness(int index);
    void highLight(int row, int col, SDL_Color color);
    bool checkVictory();
    void drawVictoryMessage(const std::string& text, const std::string& subtext);
    void playVictorySound(); 
    void highlightSameNum();
    void drawMainMenu();
    void drawDifficultyMenu();
};
