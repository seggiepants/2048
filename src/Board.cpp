#include <cstdlib>
#include <list>
#include <string>
#include <tuple>
#include <SDL_render.h>
#include "Board.h"
#include "Draw.h"
#include "Tile.h"

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

        if (this->board[y][x].value == EMPTY_SQUARE)
        {
            this->board[y][x].value = 2;
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
            this->next[y][x] = EMPTY_SQUARE;
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
            if (this->board[y][x].value >= 2048)
            {
                return true;
            }
        }
    }
    return false;
}

bool Board::IsAnimating()
{
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            if (this->board[y][x].IsMoving())
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
            if (this->board[y][x].value == EMPTY_SQUARE)
            {
                return false;
            }
        }
    }
    return true;
}

void Board::Draw(SDL_Renderer* render, TTF_Font* font, int x, int y, int w, int h)
{
    SDL_Color current;
    SDL_Rect r;
    SDL_Color face = {127, 127, 127, 255};
    SDL_Color shadow = {32, 32, 32, 255};
    SDL_Color highLight = {224, 224, 224, 255};

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
    bool anyTileMoving = this->IsAnimating();
    for(int j = 0; j < BOARD_SIZE; j++)
    {
        for(int i = 0; i < BOARD_SIZE; i++)
        {
            if (!anyTileMoving)
            {
                board[j][i].SetLocation(static_cast<float> (x + (stepX * i) + 1), static_cast<float>(y + (stepY * j) + 1));
                board[j][i].SetSize(stepX - 2, stepY - 2);
            }

            if (board[j][i].value != EMPTY_SQUARE)
            {
                // Don't draw if a tile that should dissappear, when it is done animating but everything isn't yet.
                if (anyTileMoving && !board[j][i].IsMoving() && board[j][i].action == DESTROY)
                    continue; 
                //SDL_Rect tileRect = {x + (stepX * i) + 1, y + (stepY * j) + 1, stepX - 2, stepY - 2};
                SDL_Rect tileRect = {board[j][i].GetX(), board[j][i].GetY(), stepX - 2, stepY - 2};
                DrawTile(render, tileRect, face, highLight, shadow);

                temp = this->board[j][i].value == EMPTY_SQUARE ? "" : std::to_string(this->board[j][i].value);
                SDL_Surface* surface = TTF_RenderText_Solid(font, temp.c_str(), white);
                if (surface != nullptr)
                {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
                    if (texture != nullptr)
                    {
                        int targetX = board[j][i].GetX(); //x + (stepX * i);
                        targetX += ((stepX - surface->w) / 2);
                        int targetY = board[j][i].GetY(); //y + (stepY * j);
                        targetY += ((stepY - surface->h) / 2);
                        SDL_Rect texture_rect = {targetX, targetY, surface->w, surface->h};
                        SDL_RenderCopy(render, texture, nullptr, &texture_rect);
                    }
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(surface);
                }            
            }
        }
    }


    SDL_SetRenderDrawColor(render, current.r, current.g, current.b, current.a);
}

void Board::CombineTiles(std::vector<Tile*>& column, std::vector<int>& columnNext)
{
    /*
    The column is a list of four tiles, Index 0 is the "bottom" or 
    the column, and tiles are pulled "down" and combine if they are the
    same. For example, combineTilesInColumn([2, BLANK, 2, BLANK])
    returns [4, BLANK, BLANK, BLANK].
    */

    // Copy on the numbers (not blanks) from column to combinedTiles
    std::vector<Tile*> combinedTiles; // A list of the non-blank tiles in column.
    int index = 0;  
    /*
    std::vector<std::tuple<int, int>> xy = {
        {column[0]->GetX(), column[0]->GetY()},
        {column[1]->GetX(), column[1]->GetY()},
        {column[2]->GetX(), column[2]->GetY()},
        {column[3]->GetX(), column[3]->GetY()}
    };
    */

    for(int i = 0; i < column.size(); i++)
    {
        Tile* item = column[i];
        if (item->value != EMPTY_SQUARE)
        {
            combinedTiles.push_back(item);
            Tile* target = column[index];
            columnNext[index] = column[i]->value;
            index++;
            //item->Animate(NONE, std::get<0>(xy[index]), std::get<1>(xy[index]));
            item->Animate(NONE, target->GetX(), target->GetY());
        }
    }
    for(int i = 0; i < column.size(); i++)
    {
        Tile* item = column[i];
        if (item->value == EMPTY_SQUARE)
        {
            combinedTiles.push_back(column[i]);
            columnNext[index] = column[i]->value;
            index++;
        }
    }

    for(int i = 0; i < BOARD_SIZE - 1; i++)
    {
        
        if (columnNext[i] == columnNext[i + 1] && columnNext[i] != EMPTY_SQUARE)
        {
            columnNext[i] *= 2;
            combinedTiles[i]->Animate(DOUBLE, combinedTiles[i]->GetX(), combinedTiles[i]->GetY()); //std::get<0>(xy[i]), std::get<1>(xy[i])); 
            //combinedTiles[i + 1]->Animate(DESTROY, std::get<0>(xy[i]), std::get<1>(xy[i])); //combinedTiles[i]->GetX(), combinedTiles[i]->GetY());
            for(int above = i + 1; above < BOARD_SIZE - 1; above++)
            {                
                columnNext[above] = columnNext[above + 1];
                if (columnNext[above] == EMPTY_SQUARE)
                    continue;
                //if (above != i + 1)
                    combinedTiles[above+1]->Animate(NONE, combinedTiles[above]->GetX(), combinedTiles[above]->GetY()); //std::get<0>(xy[above]), std::get<1>(xy[above])); 
            }
            columnNext[BOARD_SIZE - 1] = EMPTY_SQUARE;
            if (combinedTiles[BOARD_SIZE - 1]->value != EMPTY_SQUARE)
            {
                combinedTiles[BOARD_SIZE - 1]->Animate(NONE, combinedTiles[BOARD_SIZE - 2]->GetX(), combinedTiles[BOARD_SIZE - 2]->GetY()); //std::get<0>(xy[BOARD_SIZE - 2]), std::get<1>(xy[BOARD_SIZE - 2])); 
            }
        }
    }    
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

    // Clear the next board state.
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            this->next[y][x] = EMPTY_SQUARE;
        }
    }

    for(std::vector<std::tuple<int, int>> row : allColumnsSpaces)
    {
        std::tuple<int, int> first = row[0];
        std::tuple<int, int> second = row[1];
        std::tuple<int, int> third = row[2];
        std::tuple<int, int> fourth = row[3];

        std::vector<Tile*> column(BOARD_SIZE);
        std::vector<int> columnNext(BOARD_SIZE);
        column[0] = &this->board[std::get<1>(first)][std::get<0>(first)]; 
        column[1] = &this->board[std::get<1>(second)][std::get<0>(second)];
        column[2] = &this->board[std::get<1>(third)][std::get<0>(third)];
        column[3] = &this->board[std::get<1>(fourth)][std::get<0>(fourth)];
        this->CombineTiles(column, columnNext);

        this->next[std::get<1>(first)][std::get<0>(first)] = columnNext[0];
        this->next[std::get<1>(second)][std::get<0>(second)] = columnNext[1];
        this->next[std::get<1>(third)][std::get<0>(third)] = columnNext[2];
        this->next[std::get<1>(fourth)][std::get<0>(fourth)] = columnNext[3];
    }
    this->swap = this->IsAnimating();
}

void Board::NewGame()
{
    swap = false;
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
            if (this->board[y][x].value != EMPTY_SQUARE)
            {
                score+= board[y][x].value;
            }
        }
    }
    return score;
}

void Board::Update(float dt, std::list<Particle*>* particles)
{
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            if (this->board[y][x].IsMoving())
            {
                board[y][x].Update(dt, this, particles);
            }
        }
    }
    if (!this->IsAnimating() && this->swap)
    {
        for(int y = 0; y < BOARD_SIZE; y++)
        {
            for(int x = 0; x < BOARD_SIZE; x++)
            {
                this->board[y][x].value = this->next[y][x];
            }
        }
        this->swap = false;
        this->AddNewTwos();
    }
}

void Board::GetCellIndex(Tile* tile, int* i, int* j)
{
    for(int y = 0; y < BOARD_SIZE; y++)
    {
        for(int x = 0; x < BOARD_SIZE; x++)
        {
            if (&this->board[y][x] == tile)
            {
                *i = x;
                *j = y;
                return;
            }
        }
    }
}