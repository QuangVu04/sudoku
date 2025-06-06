#include "Board.h"
#include "Storage.h"
#include "Timer.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Board::Board(SDL_Renderer* renderer, SDL_Texture* muteTexture, Timer* timer, int windowWidth, int windowHeight)
    : renderer(renderer), muteTexture(muteTexture), selectedRow(-1), selectedCol(-1), font(nullptr), timer(timer), hoverCol(-1), hoverRow(-1),showVictoryMessage(false), victorySound(nullptr) {
    std::string basePath(SDL_GetBasePath());
    std::string fontPath = basePath + "assets/font.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 28);

    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
    }

    paddingLeft = (windowWidth - BOARD_SIZE) / 2; 
    paddingTop = PADDING_TOP;

    buttons.emplace_back(new Button("Check", 440, 540, 100, 50));
    buttons.emplace_back(new Button("New Game", 300, 540, 140, 50));
    buttons.emplace_back(new Button("Hints", 200, 540, 100, 50));
    buttons.emplace_back(new Button(muteTexture, windowWidth - 100, 0, 75, 40));
    buttons.emplace_back(new Button("Back", 10, 0, 75, 40));
    menuButtons.emplace_back(new Button("New Game", 175, 200, 200, 50));
    menuButtons.emplace_back(new Button("Load Game", 175, 270, 200, 50));
    menuButtons.emplace_back(new Button("Exit", 175, 340, 200, 50));
    levelButtons.emplace_back(new Button("Easy", 175, 200, 200, 50));
    levelButtons.emplace_back(new Button("Medium", 175, 270, 200, 50));
    levelButtons.emplace_back(new Button("Hard", 175, 340, 200, 50));
    levelButtons.emplace_back(new Button("Back", 175, 410, 200, 50));

    std::string victorySoundPath = basePath + "assets/victory_sound.wav";
    victorySound = Mix_LoadWAV(victorySoundPath.c_str());
    if (!victorySound) {
        std::cerr << "Failed to load victory sound: " << Mix_GetError() << "\n";
    }

    loadFromFile();
}

Board::~Board() {
    saveToFile();
    if (font) TTF_CloseFont(font);
}

void Board::loadFromFile() {
    std::string basePath(SDL_GetBasePath());
    Uint32 elapsedSeconds = 0;
    std::string fontPath = basePath + "assets/saved_game.txt";
    Storage::load(fontPath.c_str(), board, fixed, elapsedSeconds);
    timer->setOffset(elapsedSeconds);
}

void Board::saveToFile() {
    std::string basePath(SDL_GetBasePath());
    std::string fontPath = basePath + "assets/saved_game.txt";
    Storage::save(fontPath.c_str(), board, fixed,timer->getTime());
}

void Board::render() {
     switch (currentState) {
        case STATE_MENU:
            drawMainMenu();
            break;

        case STATE_LEVEL:
            drawDifficultyMenu();
            break;

        case STATE_PLAYING:
            timer->render(renderer);
            drawGrid();
            highlightSameNum();

            for (auto& button : buttons) {
                button->render(renderer);
            }

            highLight(hoverRow, hoverCol, {169, 169, 169, 128});
            drawNumbers();

            if (showVictoryMessage) {
                drawVictoryMessage("VICTORY!", "Congratulations! You solved the puzzle!");
            }

            if (timer->getTime() >= 600) {
                drawVictoryMessage("TIME UP!", "You ran out of time!");
            }
            break;
    }
}
    

void Board::handleEvent(SDL_Event& e) {
    switch (currentState) {
    case STATE_MENU:
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x = e.button.x;
            int y = e.button.y;

            if (menuButtons[0]->isClicked(x, y)) {
                currentState = STATE_LEVEL;
            } else if (menuButtons[1]->isClicked(x, y)) {
                loadFromFile();
                currentState = STATE_PLAYING;
            } else if (menuButtons[2]->isClicked(x, y)) {
                currentState = STATE_EXIT;
            }
        }
        break;

    case STATE_LEVEL:
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x = e.button.x;
            int y = e.button.y;

            if (levelButtons[0]->isClicked(x, y)) {
                difficultyLevel = 36 + rand() % (44 - 36 + 1);
                Utils::generatePuzzle(difficultyLevel);
                for (int i = 0; i < 9; ++i)
                    for (int j = 0; j < 9; ++j) {
                        board[i][j] = Utils::puzzle[i][j];
                        fixed[i][j] = board[i][j] != 0;
                    }
                timer->setOffset(0);
                currentState = STATE_PLAYING;
            } else if (levelButtons[1]->isClicked(x, y)) {
                difficultyLevel = 45 + rand() % (49 - 45 + 1);
                Utils::generatePuzzle(difficultyLevel);
                for (int i = 0; i < 9; ++i)
                    for (int j = 0; j < 9; ++j) {
                        board[i][j] = Utils::puzzle[i][j];
                        fixed[i][j] = board[i][j] != 0;
                    }
                timer->setOffset(0);
                currentState = STATE_PLAYING;
            } else if (levelButtons[2]->isClicked(x, y)) {
                difficultyLevel = 50 + rand() % (54 - 50 + 1);
                Utils::generatePuzzle(difficultyLevel);
                for (int i = 0; i < 9; ++i)
                    for (int j = 0; j < 9; ++j) {
                        board[i][j] = Utils::puzzle[i][j];
                        fixed[i][j] = board[i][j] != 0;
                    }
                timer->setOffset(0);
                currentState = STATE_PLAYING;
            } else if (levelButtons[3]->isClicked(x, y)) {
                currentState = STATE_MENU;
            }
        }
        break;

    case STATE_PLAYING:
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (showVictoryMessage) {
                showVictoryMessage = false;
                Utils::generatePuzzle(difficultyLevel);
                for (int i = 0; i < 9; ++i) {
                    for (int j = 0; j < 9; ++j) {
                        board[i][j] = Utils::puzzle[i][j];
                        fixed[i][j] = Utils::puzzle[i][j] != 0;
                    }
                }
                wrongCells.clear();
                selectedRow = -1;
                selectedCol = -1;
                selectedValue = 0;
                timer->setOffset(0);
                return;
            }

            int x = e.button.x;
            int y = e.button.y;

            if (y > paddingTop && y < paddingTop + BOARD_SIZE
                && x > paddingLeft && x < paddingLeft + BOARD_SIZE) {
                wrongCells.clear();
                selectedRow = (y - paddingTop - 6) / CELL_SIZE;
                selectedCol = (x - paddingLeft - 6) / CELL_SIZE;
                selectedValue = board[selectedRow][selectedCol];
            } else if (buttons[0]->isClicked(x, y)) {
                wrongCells = Utils::checkWrongCells(board);
                showVictoryMessage = checkVictory();
            } else if (buttons[4]->isClicked(x, y)) {
               currentState = STATE_MENU;
               saveToFile();
            } else if (buttons[3]->isClicked(x, y)) {
                if (bgmMuted) {
                    Mix_ResumeMusic();
                    bgmMuted = false;
                    buttons[3]->setImage({0, 0, 425, 360});
                } else {
                    Mix_PauseMusic();
                    bgmMuted = true;
                    buttons[3]->setImage({540, 0, 398, 360});
                }
            } else if (buttons[2]->isClicked(x, y)) {
                if (selectedRow >= 0 && selectedCol >= 0) {
                    if (fixed[selectedRow][selectedCol]) {
                        return;
                    }
                    int hintValue = Utils::getHint(selectedRow, selectedCol);
                    if (hintValue != -1) {
                        board[selectedRow][selectedCol] = hintValue;
                        fixed[selectedRow][selectedCol] = true;
                        highLight(selectedRow, selectedCol, {0, 255, 0, 128});
                    }
                }
            } else if (buttons[1]->isClicked(x, y)) {
                Utils::generatePuzzle(difficultyLevel);
                for (int i = 0; i < 9; ++i) {
                    for (int j = 0; j < 9; ++j) {
                        board[i][j] = Utils::puzzle[i][j];
                        fixed[i][j] = Utils::puzzle[i][j] != 0;
                    }
                }
                wrongCells.clear();
                selectedRow = -1;
                selectedCol = -1;
                selectedValue = 0;
                timer->setOffset(0);
            } else {
                wrongCells.clear();
                selectedRow = -1;
                selectedCol = -1;
                selectedValue = 0;
            }
        } else if (e.type == SDL_MOUSEMOTION) {
            int x = e.motion.x;
            int y = e.motion.y;
            if (y > paddingTop && y < paddingTop + BOARD_SIZE
                && x > paddingLeft && x < paddingLeft + BOARD_SIZE) {
                hoverRow = (y - paddingTop - 9) / CELL_SIZE;
                hoverCol = (x - paddingLeft - 9) / CELL_SIZE;
            } else {
                hoverCol = -1;
                hoverRow = -1;
            }
        } else if (e.type == SDL_KEYDOWN) {
            if (selectedRow >= 0 && selectedCol >= 0 && !fixed[selectedRow][selectedCol]) {
                int num = e.key.keysym.sym - SDLK_0;
                if (num >= 1 && num <= 9) {
                    board[selectedRow][selectedCol] = num;
                    if (checkVictory()) {
                        showVictoryMessage = true;
                        playVictorySound();
                        timer->stopTimer();
                    }
                } else if (e.key.keysym.sym == SDLK_BACKSPACE || e.key.keysym.sym == SDLK_DELETE) {
                    board[selectedRow][selectedCol] = 0;
                }
            }
        }
        break;

    case STATE_EXIT:
    SDL_Quit();
    exit(0);
        break;

    default:
        break;
}
}


void Board::playVictorySound() {
    if (victorySound) {
        Mix_PlayChannel(-1, victorySound, 0);
    }
}


void Board::drawGrid() {
    int vX = paddingLeft;
    int hY = paddingTop;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i <= 9; ++i) {
        int thickness = (i % 3 == 0) ? 3 : 1;
        SDL_Rect hLine = {paddingLeft, hY, BOARD_SIZE, thickness};
        SDL_Rect vLine = {vX, paddingTop, thickness, BOARD_SIZE};
        SDL_RenderFillRect(renderer, &hLine);
        SDL_RenderFillRect(renderer, &vLine);
        vX = vX + CELL_SIZE + thickness;
        hY = hY + CELL_SIZE + thickness;
    }
}

void Board::highlightSameNum(){
    if(selectedValue == 0) return;
    if (selectedRow != -1 && selectedCol != -1) {
        for (int row = 0; row < 9; ++row) {
            highLight(row, selectedCol, {169, 169, 169, 128}); 
        }
        for (int col = 0; col < 9; ++col) {
            highLight(selectedRow, col, {169, 169, 169, 128}); 
        }
        int startRow = (selectedRow / 3) * 3;
        int startCol = (selectedCol / 3) * 3;
        for (int i = 0; i < 3; ++i){
            for (int j = 0; j < 3; ++j)
                highLight(startRow + i, startCol + j, {169, 169, 169, 128}); 
        }
        for(int row = 0; row < 9; ++row){
            for(int col = 0; col < 9; ++col){
                if(board[row][col] == selectedValue && (row != selectedRow || col != selectedCol)){
                    highLight(row, col, {116,116,116, 128}); 
                }
            }
        }
    }

}


void Board::drawNumbers() {
    SDL_Color color = {0, 0, 0, 255};  
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (row == selectedRow && col == selectedCol) {
                highLight(row, col, {116,116,116, 128}); 
            }
            if (board[row][col] == selectedValue && selectedValue != 0) {
                if (row == selectedRow || col == selectedCol) {
                    highLight(row, col, {116,116,116, 128}); 
                }
            }
            if (!wrongCells.empty()) {
                for (const auto& cell : wrongCells) {
                    if (row == cell.first && col == cell.second) {
                        highLight(row, col, {255, 0, 0, 255}); 
                    }
                }
            }
            if (board[row][col] != 0) {
                std::stringstream ss;
                ss << board[row][col];
                SDL_Surface* surface = TTF_RenderText_Solid(font, ss.str().c_str(), color);  
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);  

                int x = paddingLeft + col * CELL_SIZE + getOffsetWithThickness(col) + 20;  
                int y = paddingTop + row * CELL_SIZE + getOffsetWithThickness(row) + 10;  

                SDL_Rect dst = {x, y, surface->w, surface->h};  
                SDL_RenderCopy(renderer, texture, NULL, &dst);  

               
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }
}


void Board::highLight(int row, int col, SDL_Color color) {
    if (row < 0 || col < 0 || row >= 9 || col >= 9) return;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect highlightRect = {
        paddingLeft + col * CELL_SIZE + getOffsetWithThickness(col),
        paddingTop + row * CELL_SIZE + getOffsetWithThickness(row),
        CELL_SIZE,
        CELL_SIZE
    };
    SDL_RenderFillRect(renderer, &highlightRect);
}

int Board::getOffsetWithThickness(int index) {
    int thickCount = index / 3 + 1;
    int thinCount = index + 1 - thickCount;
    return thickCount * 3 + thinCount * 1;
}
bool Board::checkVictory() {
    
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] == 0) {
                return false; 
            }
        }
    }
    
    
    std::vector<std::pair<int, int>> errors = Utils::checkWrongCells(board);
    return errors.empty();
}

void Board::drawVictoryMessage(const std::string& text, const std::string& subtext) {
    timer->stopTimer();
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect bgRect = {0, 0, 550, 600}; 
    SDL_RenderFillRect(renderer, &bgRect);
    
    
    SDL_Color color = {255, 215, 0, 255}; 
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int x = (550 - surface->w) / 2;
    int y = (600 - surface->h) / 2 - 30;
    
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
    
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    color = {255, 255, 255, 255}; 
    
    surface = TTF_RenderText_Solid(font, subtext.c_str(), color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    x = (550 - surface->w) / 2;
    y = (600 - surface->h) / 2 + 20;
    
    dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
    
    std::stringstream ss;
    Uint32 totalSeconds = timer->getTime();
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    
    ss << "Time: " << std::setw(2) << std::setfill('0') << minutes
       << ":" << std::setw(2) << std::setfill('0') << seconds;
    
    surface = TTF_RenderText_Solid(font, ss.str().c_str(), color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    x = (550 - surface->w) / 2;
    y = (600 - surface->h) / 2 + 60;
    
    dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Board::drawMainMenu() {
    SDL_SetRenderDrawColor(renderer, 255,255,255, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Sudoku Game", color);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { (550 - titleSurface->w) / 2, 100, titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    for (auto& button : menuButtons) {
        button->render(renderer);
    }
}

void Board::drawDifficultyMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Select Difficulty", color);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { (550 - titleSurface->w) / 2, 100, titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    for (auto& button : levelButtons) {
        button->render(renderer);
    }
}