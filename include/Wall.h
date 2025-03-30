#ifndef WALL_H
#define WALL_H

#include <SDL.h>
#include "constants.h"

class Wall {
public:
    SDL_Rect rect;
    SDL_Color color;
    Wall(int x, int y);
    void render(SDL_Renderer* renderer);
};

#endif // WALL_H
