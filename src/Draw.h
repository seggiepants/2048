#ifndef __DRAW_H__

#define __DRAW_H__

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#define BORDER_WIDTH 3

void DrawMessage(SDL_Renderer* render, TTF_Font* font, std::string message);
void DrawTile(SDL_Renderer* render, SDL_Rect& r, SDL_Color face, SDL_Color highLight, SDL_Color shadow);

#endif