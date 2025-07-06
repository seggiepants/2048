#include <cstdlib>
#include <list>
#include <string>
#include <tuple>
#include <SDL_render.h>
#include "Board.h"

Board::Board()
{
    this->NewGame();
}

void Board::AddNewTwos(int count)
{
    int countRemaining = count;
    while (countRemaining > 0)
    {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;

        if (this->board[y][x] == EMPTY_SQUARE)
        {
            this->board[y][x] = 2;
            countRemaining--;
        }
    }
}

void Board::Clear()
{
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        black.r = black.g = black.b = 0;
        white.r = white.g = white.b = 255;
        black.a = white.a = 255;

        for(int x = 0; x < BOARD_SIZE; x++)
        {
            this->Set(x, y, EMPTY_SQUARE);
        }
    }
}

bool Board::HasWon()
{
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            if (this->board[y][x] >= 2048)
            {
                return true;
            }
        }
    }
    return false;
}

bool Board::IsFull()
{
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            if (this->board[y][x] == EMPTY_SQUARE)
            {
                return false;
            }
        }
    }
    return true;
}

void Board::Draw(SDL_Renderer* render, TTF_Font* font, int x, int y, int w, int h)
{
    struct SDL_Color current;
    struct SDL_Rect r;
    SDL_GetRenderDrawColor(render, &current.r, &current.g, &current.b, &current.a);
    SDL_SetRenderDrawColor(render, black.r, black.g, black.b, black.a);
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_RenderFillRect(render, &r);
    SDL_SetRenderDrawColor(render, white.r, white.g, white.b, white.a);
    SDL_RenderDrawRect(render, &r);
    int stepX = r.w / BOARD_SIZE;
    int stepY = r.h / BOARD_SIZE;
    for(int i = 1; i < BOARD_SIZE; i++)
    {
        SDL_RenderDrawLine(render, r.x + stepX * i, r.y, r.x + stepX * i - 1, r.y + r.h - 1);
        SDL_RenderDrawLine(render, r.x, r.y + stepY * i, r.x + r.w - 1, r.y + stepY * i - 1);
    }
    std::string temp;
    for(int j = 0; j < BOARD_SIZE; j++)
    {
        for(int i = 0; i < BOARD_SIZE; i++)
        {
            temp = this->board[j][i] == EMPTY_SQUARE ? "" : std::to_string(this->board[j][i]);
            SDL_Surface* surface = TTF_RenderText_Solid(font, temp.c_str(), white);
            if (surface != nullptr)
            {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
                if (texture != nullptr)
                {
                    int targetX = x + (stepX * i);
                    targetX += ((stepX - surface->w) / 2);
                    int targetY = y + (stepY * j);
                    targetY += ((stepY - surface->h) / 2);
                    SDL_Rect texture_rect = {targetX, targetY, surface->w, surface->h};
                    SDL_RenderCopy(render, texture, nullptr, &texture_rect);
                }
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }            
        }
    }


    SDL_SetRenderDrawColor(render, current.r, current.g, current.b, current.a);
}

std::vector<int>* Board::CombineTiles(std::vector<int>& column)
{
    /*
    The column is a list of four tiles, Index 0 is the "bottom" or 
    the column, and tiles are pulled "down" and combine if they are the
    same. For example, combineTilesInColumn([2, BLANK, 2, BLANK])
    returns [4, BLANK, BLANK, BLANK].
    */

    // Copy on the numbers (not blanks) from column to combinedTiles
    std::vector<int> combinedTiles; // A list of the non-blank tiles in column.        
    for(int item : column)
    {
        if (item != EMPTY_SQUARE)
        {
            combinedTiles.push_back(item);
        }
    }
    while (combinedTiles.size() < BOARD_SIZE)
    {
        combinedTiles.push_back(EMPTY_SQUARE);
    }

    for(int i = 0; i < BOARD_SIZE - 1; i++)
    {
        if (combinedTiles[i] == combinedTiles[i + 1] && combinedTiles[i] != EMPTY_SQUARE)
        {
            combinedTiles[i] *= 2;
            for(int above = i + 1; above < BOARD_SIZE - 1; above++)
            {
                combinedTiles[above] = combinedTiles[above + 1];
            }
            combinedTiles[BOARD_SIZE - 1] = EMPTY_SQUARE;
        }
    }
    return new std::vector<int>(combinedTiles.begin(), combinedTiles.end());    
}

void Board::Move(Direction dir)
{
    std::vector<std::vector<std::tuple<int, int>>> allColumnsSpaces;
    if (dir == UP)
    {
        allColumnsSpaces = {{{0, 0}, {0, 1}, {0, 2}, {0, 3}},
                            {{1, 0}, {1, 1}, {1, 2}, {1, 3}}, 
                            {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, 
                            {{3, 0}, {3, 1}, {3, 2}, {3, 3}}};
    }
    else if (dir == LEFT)
    {        
        allColumnsSpaces = {{{0, 0}, {1, 0}, {2, 0}, {3, 0}},
                            {{0, 1}, {1, 1}, {2, 1}, {3, 1}}, 
                            {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, 
                            {{0, 3}, {1, 3}, {2, 3}, {3, 3}}};
    } 
    else if (dir == DOWN)
    {
        allColumnsSpaces = {{{0, 3}, {0, 2}, {0, 1}, {0, 0}},
                            {{1, 3}, {1, 2}, {1, 1}, {1, 0}}, 
                            {{2, 3}, {2, 2}, {2, 1}, {2, 0}}, 
                            {{3, 3}, {3, 2}, {3, 1}, {3, 0}}};

    }
    else if (dir == RIGHT)
    {
        allColumnsSpaces = {{{3, 0}, {2, 0}, {1, 0}, {0, 0}},
                            {{3, 1}, {2, 1}, {1, 1}, {0, 1}}, 
                            {{3, 2}, {2, 2}, {1, 2}, {0, 2}}, 
                            {{3, 3}, {2, 3}, {1, 3}, {0, 3}}};
    }

    // ZZZ - More!
    Board* next = new Board();
    next->Clear();
    for(std::vector<std::tuple<int, int>> row : allColumnsSpaces)
    {
        std::tuple<int, int> first = row[0];
        std::tuple<int, int> second = row[1];
        std::tuple<int, int> third = row[2];
        std::tuple<int, int> fourth = row[3];

        std::vector<int> column(BOARD_SIZE);
        column[0] = this->Get((std::get<0>(first)), (std::get<1>(first)));
        column[1] = this->Get((std::get<0>(second)), (std::get<1>(second)));
        column[2] = this->Get((std::get<0>(third)), (std::get<1>(third)));
        column[3] = this->Get((std::get<0>(fourth)), (std::get<1>(fourth)));
        std::vector<int>* combined = this->CombineTiles(column);

        next->Set(std::get<0>(first), std::get<1>(first), (*combined)[0]);
        next->Set(std::get<0>(second), std::get<1>(second), (*combined)[1]);
        next->Set(std::get<0>(third), std::get<1>(third), (*combined)[2]);
        next->Set(std::get<0>(fourth), std::get<1>(fourth), (*combined)[3]);
        delete combined;
    }

    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE;x++)
        {
            this->Set(x, y, next->Get(x, y));
        }
    }
    delete next;
}

void Board::NewGame()
{
    this->Clear();
    this->AddNewTwos(2);
}

int Board::Score()
{
    int score = 0;
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            if (this->board[y][x] != EMPTY_SQUARE)
            {
                score+= board[y][x];
            }
        }
    }
    return score;
}