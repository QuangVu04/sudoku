#include "Game.h"
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

Game::Game() : window(nullptr), renderer(nullptr), running(false), board(nullptr), timer(nullptr), font(nullptr), muteTexture(nullptr), bgm(nullptr), victorySound(nullptr) {}

Game::~Game() {}

bool Game::init(const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || TTF_Init() < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer Init Error: " << Mix_GetError() << "\n";
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image không thể khởi tạo! IMG_Error: " << IMG_GetError() << "\n";
        return 1;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        std::cerr << "Window/Renderer Init Error\n";
        return false;
    }
    
    std::string basePath(SDL_GetBasePath());

    std::string muteImagePath = basePath + "assets/mute.jpg";
    SDL_Surface* loadedSurface = IMG_Load(muteImagePath.c_str());
    if (loadedSurface == nullptr) {
        std::cout << "Không thể tải ảnh! IMG_Error: " << IMG_GetError() << "\n";
        return 1;
    }

    muteTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    if (muteTexture == nullptr) {
        std::cout << "Không thể tạo texture! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    timer = new Timer();
    board = new Board(renderer, muteTexture, timer, WINDOW_WIDTH, WINDOW_HEIGHT);
    std::string fontPath = basePath + "assets/font.ttf";
    
    font = TTF_OpenFont(fontPath.c_str(), 75);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
    }

    std::string musicPath = basePath + "assets/sound_game.wav";
    bgm = Mix_LoadMUS(musicPath.c_str());
    if (!bgm) {
    std::cerr << "Failed to load music: " << Mix_GetError() << "\n";
    } else {
    Mix_PlayMusic(bgm, -1); 
    }

    std::string victorySoundPath = basePath + "assets/victory_sound.wav";
    victorySound = Mix_LoadWAV(victorySoundPath.c_str());
    if (!victorySound) {
        std::cerr << "Failed to load victory sound: " << Mix_GetError() << "\n";

    }

    running = true;
    return true;
}

void Game::drawTitle(std::string title) {
    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, title.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {(WINDOW_WIDTH - surface->w) / 2, -35, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::run() {
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            board->handleEvent(e);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        drawTitle("sudoku");
        board->render();
        // timer->render(renderer);

        SDL_RenderPresent(renderer);
    }
}

void Game::clean() {
    delete board;
    delete timer;
    if (font) TTF_CloseFont(font);
    Mix_CloseAudio();
    SDL_DestroyTexture(muteTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
