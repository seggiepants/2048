#ifndef __BOARD_H__
#define __BOARD_H__
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Tile.h"

#define BOARD_SIZE 4
#define EMPTY_SQUARE 0

enum Direction
{
    UP, DOWN, LEFT, RIGHT
};

class Board
{
    public:
        Board();
        void AddNewTwos(int count = 1);
        void Clear();
        std::vector<int>* CombineTiles(std::vector<int>& column);
        void Draw(SDL_Renderer* render, TTF_Font* font, int x, int y, int w, int h);
        int Get(int x, int y) { return board[y][x].value; }
        bool HasWon();
        bool IsFull();
        void Move(Direction dir);
        void NewGame();
        int Score();
        void Set(int x, int y, int value) { board[y][x].value = value; }
    private:
        Tile board[BOARD_SIZE][BOARD_SIZE];
        struct SDL_Color black, white;
};

#endif