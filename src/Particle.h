#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <SDL.h>

#define LIFETIME_MAX 1000
#define SPEED_MAX 300

class Particle
{
public:
    Particle(int x, int y);
    void Update(float dt, int screenW, int screenH);
    void Draw(SDL_Renderer* render, SDL_Texture* tiles);
    bool IsAlive() {return alive;};
    float x;
    float y;
private:
    float angle;
    bool alive;
    float lifeTime;
    float speed;
};

#endif