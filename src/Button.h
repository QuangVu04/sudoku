#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include<iostream>
#include<string>

class Button {
public:
    Button(SDL_Texture* texture, int x, int y, int width, int height);
    Button(std::string text, int x, int y, int width, int height);
    ~Button();

    void render(SDL_Renderer* renderer);
    bool isClicked(int mouseX, int mouseY);
    void setImage(SDL_Rect rect);

private:
    std::string text;
    SDL_Texture* texture = nullptr;
    SDL_Rect srcRect;
    int x, y, width, height;
    TTF_Font* font;
};
