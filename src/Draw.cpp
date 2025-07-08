#include "Draw.h"

void DrawMessage(SDL_Renderer* render, TTF_Font* font, std::string message)
{
    struct SDL_Color black = {0, 0, 0, 255};
    struct SDL_Color white = {255, 255, 255, 255};
    struct SDL_Rect r;
    SDL_SetRenderDrawColor(render, black.r, black.g, black.b, black.a);
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), black);
    if (surface != nullptr)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
        if (texture != nullptr)
        {
            int screenW, screenH;
            if (SDL_GetRendererOutputSize(render, &screenW, &screenH) == 0)
            {
                int targetX = (screenW - surface->w) / 2;
                int targetY = (screenH - surface->h) / 2;                
                SDL_Rect r = {targetX, targetY, surface->w, surface->h};
                SDL_SetRenderDrawColor(render, white.r, white.g, white.b, white.a);
                SDL_RenderFillRect(render, &r);
                
                SDL_RenderCopy(render, texture, nullptr, &r);
            }
        }
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }            
}

void DrawTile(SDL_Renderer* render, SDL_Rect& r, SDL_Color face, SDL_Color highLight, SDL_Color shadow)
{
    SDL_SetRenderDrawColor(render, highLight.r, highLight.g, highLight.b, highLight.a);
    SDL_Rect tmp = {r.x, r.y, r.w, BORDER_WIDTH};
    SDL_RenderFillRect(render, &tmp);
    tmp.x = r.x + r.w - BORDER_WIDTH;
    tmp.y = r.y + BORDER_WIDTH;
    tmp.w = BORDER_WIDTH;
    tmp.h = r.h - BORDER_WIDTH;
    SDL_RenderFillRect(render, &tmp);
    SDL_SetRenderDrawColor(render, shadow.r, shadow.g, shadow.b, shadow.a);
    tmp.x = r.x;
    tmp.y = r.y + r.h - BORDER_WIDTH;
    tmp.w = r.w;
    tmp.h = BORDER_WIDTH;
    SDL_RenderFillRect(render, &tmp);
    tmp.x = r.x;
    tmp.y = r.y + BORDER_WIDTH;
    tmp.w = BORDER_WIDTH;
    tmp.h = r.h - BORDER_WIDTH;
    SDL_RenderFillRect(render, &tmp);
    SDL_SetRenderDrawColor(render, face.r, face.g, face.b, face.a);
    tmp.x = r.x + BORDER_WIDTH;
    tmp.y = r.y + BORDER_WIDTH;
    tmp.w = r.w - (2 * BORDER_WIDTH);
    tmp.h = r.h - (2 * BORDER_WIDTH);
    SDL_RenderFillRect(render, &tmp);

}
