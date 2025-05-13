#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Board.h"
#include "Timer.h"
#include <string>
// #include <SDL_mixer.h>

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title);
    void drawTitle(std::string title);
    void run();
    void clean();

private:
    const int WINDOW_WIDTH = 550;
    const int WINDOW_HEIGHT = 600;

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    bool running;
    // Mix_Music *bgm = nullptr;
    // Mix_Chunk *gClick = nullptr;

    Board* board;
    Timer* timer;
};
