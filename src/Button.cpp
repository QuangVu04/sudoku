#include "Button.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <sstream>
#include <iomanip>

Button::Button(std::string text, int x, int y, int w, int h)
    : text(text), x(x), y(y), width(w), height(h) {
    std::string basePath(SDL_GetBasePath());
    std::string fontPath = basePath + "assets/font.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 28);
}

Button::~Button() {
    if (font) TTF_CloseFont(font);
}

void Button::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
    SDL_Rect borderRect = {x, y, width, height};
    SDL_RenderDrawRect(renderer, &borderRect);

    
    SDL_Color color = {0, 0, 0, 255};  
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    
    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_Rect dstRect;
    dstRect.x = x + (width - textWidth) / 2;  
    dstRect.y = y + (height - textHeight) / 2; 
    dstRect.w = textWidth;
    dstRect.h = textHeight;

   
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool Button::isClicked(int mouseX, int mouseY) {
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}
