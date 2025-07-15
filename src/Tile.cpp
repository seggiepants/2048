#include "Tile.h"
#include "Board.h"

#define TILE_SPEED 600
#define PARTICLE_COUNT 6

Tile::Tile()
{
    this->x = this->y = 0.0f;
    this->targetX = this->targetY = 0.0f;
    this->action = DESTROY;
    this->animate = false;
    this->w = this->h = 0;
    this->value = EMPTY_SQUARE;
}

void Tile::Animate(AnimationCompleteAction action, float targetX, float targetY)
{
    // User should set original x, y screen location before animating.
    // that way if we multi-merge we keep going to the desired location.
    this->targetX = targetX;
    this->targetY = targetY;
    this->action = action;
    this->animate = true;
}

void Tile::Update(float dt, Board* board, std::list<Particle*>* particles)
{
    // Only update if animating.
    if (this->animate)
    {
        float oldX = this->x;
        float oldY = this->y;
        float x1, y1, x2, y2;

        float dx, dy;
        // Can really only move in one direction up, down, left, or right.
        if (this->x == this->targetX)
            dx = 0.0f;
        else if (this->x < this->targetX)
            dx = 1.0f;
        else
            dx = -1.0f;

        if (this->y == this->targetY)
            dy = 0.0f;
        else if (this->y < this->targetY)
            dy = 1.0f;
        else
            dy = -1.0f;

        // Move the tile.
        this->x += dt * TILE_SPEED * dx;
        this->y += dt * TILE_SPEED * dy;

        // If we passed through the target we are at the target.
        // Find the bounding box of old location to current.
        x1 = oldX < this->x ? oldX : this->x;
        x2 = oldX > this->x ? oldX : this->x;
        y1 = oldY < this->y ? oldY : this->y;
        y2 = oldY > this->y ? oldY : this->y;
        
        // If targetX, targetY is in the bounding box we are at targetX, and targetY and 
        // should stop animating. Maybe toss out some particle effects.
        if (targetX >= x1 && targetX <= x2 && targetY >= y1 && targetY <= y2)
        {
            // Done animating.
            this->x = targetX;
            this->y = targetY;
            this->animate = false;

            if (this->action != NONE)
            {
                // Add a particle effect.
                for(int i = 0; i < PARTICLE_COUNT; i++)
                {
                    Particle *p = new Particle(this->x + (this->w / 2) + (PARTICLE_SIZE / 2), this->y + (this->h / 2) - (PARTICLE_SIZE / 2));
                    particles->push_back(p);
                }
            }
        }

    }
}    