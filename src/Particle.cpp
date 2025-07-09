#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include "Particle.h"

Particle::Particle(int x, int y)
{
    this->x = static_cast<float>(x);
    this->y = static_cast<float>(y);
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    this->angle = 2.0 * M_PI * r;
    this->lifeTime = static_cast<float>((LIFETIME_MAX / 2) + (rand() % (LIFETIME_MAX / 2))); //(0.5 - 1.0 of LIFETIME_MAX)
    this->speed = static_cast<float>((SPEED_MAX / 2) + (rand() % (SPEED_MAX / 2))); //(0.5 - 1.0 of SPEED_MAX)
    this->alive = true;
}

void Particle::Update(float dt, int screenW, int screenH)
{
    const float MILLISEC_PER_SECOND = 1000.0f;
    this->x += (this->speed * std::cos(this->angle) * dt) / MILLISEC_PER_SECOND;
    this->y += (this->speed * std::sin(this->angle) * dt) / MILLISEC_PER_SECOND;

    if (this->x < 0.0f || this->y < 0.0f || this->x > static_cast<float>(screenW) || this->y > static_cast<float>(screenH))
    {
        this->alive = false; // remove particle when off screen.
    }

    if (this->lifeTime > 0.0f && this->alive)
    {
        this->lifeTime -= dt;
        if (this->lifeTime <= 0.0f)
            this->alive = false;
    }
}

void Particle::Draw(SDL_Renderer* render, SDL_Texture* tiles)
{

}