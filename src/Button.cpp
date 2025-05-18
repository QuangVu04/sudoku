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
    if (!font) {
        std::cerr << "Lỗi mở font: " << TTF_GetError() << std::endl;
    }
    texture = nullptr;
    srcRect = {0, 0, 0, 0};
}

Button::Button(SDL_Texture* texture, int x, int y, int w, int h) 
    : texture(texture), x(x), y(y), width(w), height(h) {
        text = "";
        font = nullptr;
        srcRect = {0, 0, 425, 360};
    }

Button::~Button() {
    if (font) TTF_CloseFont(font);
}

void Button::render(SDL_Renderer* renderer) {
    SDL_Rect dstRect;
    if (!text.empty()) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_Rect borderRect = {x, y, width, height};
        SDL_RenderDrawRect(renderer, &borderRect);

        SDL_Color color = {0, 0, 0, 255};  
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surface);

        int textWidth = surface->w;
        int textHeight = surface->h;
        
        dstRect.x = x + (width - textWidth) / 2;  
        dstRect.y = y + (height - textHeight) / 2; 
        dstRect.w = textWidth;
        dstRect.h = textHeight;

        SDL_RenderCopy(renderer, textureText, NULL, &dstRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(textureText);
    } else if (texture) {
        dstRect.x = x;
        dstRect.y = y;
        dstRect.w = width;
        dstRect.h = height;
        SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
    }
}

bool Button::isClicked(int mouseX, int mouseY) {
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}

void Button::setImage(SDL_Rect rect) {
    srcRect = rect;
}
