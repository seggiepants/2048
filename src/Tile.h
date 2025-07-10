#ifndef __TILE_H__

#define __TILE_H__

#include <list>
#include "Particle.h"

enum AnimationCompleteAction 
{
    DESTROY, DOUBLE
};
class Board;

class Tile
{
public:
    Tile();
    void Animate(AnimationCompleteAction action, float targetX, float targetY, int targetCellX, int targetCellY);
    bool IsMoving() { return animate; };
    void Update(float dt, Board* board, std::list<Particle*>* particles);    
    int GetX() { return static_cast<int> (this->x);};
    int GetY() { return static_cast<int> (this->y);};
    void SetLocation(float x, float y) { this->x = x; this->y = y;};
    void SetSize(int w, int h) { this->w = w; this->h = h;};
    int value; // Number at current cell.
private:
    bool animate;
    AnimationCompleteAction action; // What to do when we get there.
    float x;        // Current location.
    float y;
    float targetX;    // LERP to on the screen.
    float targetY;
    int targetCellX;  // Where to write our doubled value. 
    int targetCellY;
    int w;
    int h;

};

#endif