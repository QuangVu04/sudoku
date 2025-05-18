#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include<iostream>
#include<string>

class Button {
public:
    Button(std::string text, int x, int y, int width, int height);
    ~Button();

    void render(SDL_Renderer* renderer);
    bool isClicked(int mouseX, int mouseY);
    void setText(const std::string& newText);

private:
    std::string text;
    int x, y, width, height;
    TTF_Font* font;
};
