#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

class Timer {
public:
    Timer();
    ~Timer();

    void setOffset(Uint32 seconds); 
    Uint32 getTime() const;
    void render(SDL_Renderer* renderer);

private:
    Uint32 startTicks;
    Uint32 offsetSeconds;
    TTF_Font* font;
};
