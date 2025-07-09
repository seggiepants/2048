#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Board.h"
#include "Draw.h"
#include "Particle.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

enum GameState {
    PLAYING,
    PAUSED,
    WIN,
    LOSE
};

void Draw(SDL_Renderer* render, TTF_Font* font, SDL_Texture* tiles, GameState state, Board* board, std::list<Particle*>* particles)
{
    const int pad = 4;
    struct SDL_Color white = {255, 255, 255, 255};
    
    SDL_RenderClear(render);
    int screenW, screenH;

    if (SDL_GetRendererOutputSize(render, &screenW, &screenH) == 0)
    {
        // Draw the score, center bottom.
        std::string score = "Score: " + std::to_string(board->Score());
        SDL_Surface* surface = TTF_RenderText_Solid(font, score.c_str(), white);    

        int targetX, targetY;
        targetX = (screenW - surface->w) / 2;
        targetY = screenH - surface->h - pad;
        SDL_Rect rect = {targetX, targetY, surface->w, surface->h};
        SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);        
        if (texture != nullptr)
        {
            SDL_RenderCopy(render, texture, nullptr, &rect);
        }
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        rect.y = pad;
        rect.h = targetY - (2 * pad);
        rect.w = rect.h;
        rect.x = (screenW - rect.w) / 2;

        
        SDL_SetRenderDrawColor(render, white.r, white.g, white.b, white.a);
        board->Draw(render, font, rect.x, rect.y, rect.w, rect.h);
        
        SDL_Rect rParticle = {0, 0, 16, 16}; // ZZZ hard coded :(
        SDL_Rect rDest = {0, 0, 16, 16};
        for(std::list<Particle*>::iterator iter = particles->begin(); iter != particles->end(); iter++)
        {
            rDest.x = (*iter)->x;
            rDest.y = (*iter)->y;
            SDL_RenderCopy(render, tiles, &rParticle, &rDest);
        }

        if (state == PAUSED)
            DrawMessage(render, font, "PAUSED");
        else if (state == LOSE)
            DrawMessage(render, font, "GAME OVER");
        else if (state == WIN)
            DrawMessage(render, font, "YOU WIN!");

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

        SDL_RenderPresent(render);
    }
}


int main(int argc, char *argv[])
{    
    std::srand(std::time({})); // use current time as seed for random generator
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);
    if (TTF_Init() < 0) 
    {
        std::cout << "Error initializing SDL_ttf: " << SDL_GetError();
    }
    
    SDL_Window *window = SDL_CreateWindow(
        "2048",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );
    SDL_SetWindowResizable(window, SDL_FALSE);
    
    TTF_Font* font = TTF_OpenFont("res/font/Bitstream Vera Sans Mono Bold.ttf", 25);    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* tiles = IMG_LoadTexture(renderer, "res/img/tiles.png");
    bool running = true;
    SDL_Event event;
    unsigned int time = SDL_GetTicks();
    Board* board = new Board();
    GameState state = PLAYING;
    std::list<Particle*> particles;
    unsigned int previous = SDL_GetTicks() - 16; // -16 to make sure we have a non-zero delta time.
    while (running)
    {
        bool moved = false;
        unsigned int now = SDL_GetTicks();
        float dt = static_cast<float>(now) - static_cast<float>(previous);
        previous = now;

        // Update and remove dead particles.
        std::list<Particle*>::iterator iter = particles.begin();
        while (iter != particles.end())
        {
            (*iter)->Update(dt, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (!(*iter)->IsAlive())
            {
                delete (*iter);
                particles.erase(iter++);
            }
            else
            {
                iter++;
            }
        }

        if (SDL_PollEvent(&event) != 0)
        { 
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    {
                        int mouseX = event.motion.x;
                        int mouseY = event.motion.y;
                        Particle* p = new Particle(mouseX - 8, mouseY - 8);
                        particles.push_back(p);
                    }
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_LEFT:
                            if (state == PLAYING)
                            {
                                board->Move(LEFT);
                                moved = true;
                            }
                            break;
                        case SDLK_RIGHT:
                            if (state == PLAYING)
                            {
                                board->Move(RIGHT);
                                moved = true;
                            }
                            break;
                        case SDLK_UP:
                            if (state == PLAYING)
                            {
                                board->Move(UP);
                                moved = true;
                            }
                            break;
                        case SDLK_DOWN:
                            if (state == PLAYING)
                            {
                                board->Move(DOWN);
                                moved = true;
                            }
                            break;
                        case SDLK_p:
                            if (state == PAUSED)
                                state = PLAYING;
                            else if (state == PLAYING)
                                state = PAUSED;
                            break;
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                    }
                    break;
            }
        }
        if (moved)
        {
            board->AddNewTwos();
            if (board->HasWon())
            {
                state = WIN;

            }
            else if (board->IsFull())
            {
                state = LOSE;
            }
        }
        Draw(renderer, font, tiles, state, board, &particles);
        int ticks = now - time;
        if (ticks < SCREEN_TICKS_PER_FRAME)
        {
            // Wait remaining time.
            SDL_Delay(SCREEN_TICKS_PER_FRAME - ticks);
        }
        time = now;
    }

    std::list<Particle*>::iterator iter = particles.begin();
    while (iter != particles.end())
    {
        delete (*iter);
    }
    particles.clear();

    delete board;
    TTF_CloseFont(font);    
    TTF_Quit();
    SDL_DestroyTexture(tiles);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
