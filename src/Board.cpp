#include "Board.h"
#include "Storage.h"
#include "Timer.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Board::Board(SDL_Renderer* renderer, Timer* timer, int windowWidth, int windowHeight)
    : renderer(renderer), selectedRow(-1), selectedCol(-1), font(nullptr), timer(timer), hoverCol(-1), hoverRow(-1) {
    std::string basePath(SDL_GetBasePath());
    std::string fontPath = basePath + "assets/font.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 28);

    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
    }

    paddingLeft = (windowWidth - BOARD_SIZE) / 2; 
    paddingTop = PADDING_TOP;

    buttons.emplace_back(new Button("Check", 440, 540, 100, 50));
    buttons.emplace_back(new Button("New Game", 200, 540, 150, 50));

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
    drawGrid();

    buttons[0]->render(renderer);
    buttons[1]->render(renderer);

    highLight(hoverRow, hoverCol, {169, 169, 169, 128});
    drawNumbers();
     if (showVictoryMessage) {
        drawVictoryMessage();
    }
}

void Board::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (showVictoryMessage) {
            showVictoryMessage = false;
            return;
        }
        int x = e.button.x;
        int y = e.button.y;
        if (y > paddingTop && y < paddingTop + BOARD_SIZE
            && x > paddingLeft && x < paddingLeft + BOARD_SIZE
        ) {
            wrongCells.clear();
            selectedRow = (y - paddingTop - 6) / CELL_SIZE;
            selectedCol = (x - paddingLeft - 6) / CELL_SIZE;

            if (board[selectedRow][selectedCol] != 0) {
                selectedValue = board[selectedRow][selectedCol];
            }
        } else if (buttons[0]->isClicked(x, y)) {
            wrongCells = Utils::checkWrongCells(board);
            showVictoryMessage = checkVictory();
        } else if (buttons[1]->isClicked(x, y)) {
            Utils::generatePuzzle();
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
            && x > paddingLeft && x < paddingLeft + BOARD_SIZE
        ) {
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
                    timer->stopTimer();
                    
                }
            } else if (e.key.keysym.sym == SDLK_BACKSPACE || e.key.keysym.sym == SDLK_DELETE) {
                board[selectedRow][selectedCol] = 0;
            }
        }
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

    if (selectedRow != -1 && selectedCol != -1) {
        for (int row = 0; row < 9; ++row) {
            highLight(row, selectedCol, {169, 169, 169, 128}); 
        }
        for (int col = 0; col < 9; ++col) {
            highLight(selectedRow, col, {169, 169, 169, 128}); 
        }
    }
}

void Board::drawNumbers() {
    SDL_Color color = {0, 0, 0, 255};  
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (row == selectedRow && col == selectedCol) {
                highLight(row, col, {169, 169, 169, 255}); 
            }
            if (board[row][col] == selectedValue && selectedValue != 0) {
                if (row == selectedRow || col == selectedCol) {
                    highLight(row, col, {169, 169, 169, 255}); 
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

void Board::drawVictoryMessage() {
    timer->stopTimer();
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect bgRect = {0, 0, 550, 600}; 
    SDL_RenderFillRect(renderer, &bgRect);
    
    
    SDL_Color color = {255, 215, 0, 255}; 
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, "VICTORY!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int x = (550 - surface->w) / 2;
    int y = (600 - surface->h) / 2 - 30;
    
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
    
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    color = {255, 255, 255, 255}; 
    
    surface = TTF_RenderText_Solid(font, "Congratulations! You solved the puzzle!", color);
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