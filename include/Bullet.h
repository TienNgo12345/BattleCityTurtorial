#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>
#include "constants.h"

class Bullet {
public:
    SDL_Rect rect;
    int speed;
    int dirX, dirY;
    bool active;

    Bullet(int x, int y, int dX, int dY);
    void update();
    void render(SDL_Renderer* renderer, SDL_Texture* bulletTex);
};

#endif // BULLET_H
