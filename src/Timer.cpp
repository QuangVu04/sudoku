#include "Timer.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <sstream>
#include <iomanip>

Timer::Timer() : offsetSeconds(0) {
    startTicks = SDL_GetTicks();
    std::string basePath(SDL_GetBasePath());
    std::string fontPath = basePath + "assets/font.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 28);
}

Timer::~Timer() {
    if (font) TTF_CloseFont(font);
}

void Timer::setOffset(Uint32 seconds) {
    offsetSeconds = seconds;
    startTicks = SDL_GetTicks();
}

Uint32 Timer::getTime() const {
    return offsetSeconds + (SDL_GetTicks() - startTicks) / 1000;
}

void Timer::render(SDL_Renderer* renderer) {
    Uint32 totalSeconds = getTime();
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::stringstream ss;
    ss << "Time: " << std::setw(2) << std::setfill('0') << minutes
       << ":" << std::setw(2) << std::setfill('0') << seconds;

    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, ss.str().c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {10, 550, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Timer::stopTimer() {
    
    offsetSeconds = getTime();
    
    startTicks = SDL_GetTicks();
}