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
        void CombineTiles(std::vector<Tile*>& column, std::vector<int>& columnNext);
        void Draw(SDL_Renderer* render, TTF_Font* font, int x, int y, int w, int h);
        int Get(int x, int y) { return board[y][x].value; }
        bool HasWon();
        bool IsAnimating();
        bool IsFull();
        void Move(Direction dir);
        void NewGame();
        int Score();
        void Set(int x, int y, int value) { board[y][x].value = value;}
        void Update(float dt, std::list<Particle*>* particles);
    private:
        void GetCellIndex(Tile* tile, int* i, int* j);
        Tile board[BOARD_SIZE][BOARD_SIZE];
        int next[BOARD_SIZE][BOARD_SIZE];
        bool swap;
        struct SDL_Color black, white;
};

#endif