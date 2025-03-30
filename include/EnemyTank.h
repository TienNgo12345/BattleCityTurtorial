#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include <SDL.h>
#include <vector>
#include "Bullet.h"
#include "constants.h"

class EnemyTank {
public:
    SDL_Rect rect;
    int speed;
    int dx, dy;
    std::vector<Bullet> bullets;

    EnemyTank(int x, int y);
    void move();
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer, SDL_Texture* tankTex, SDL_Texture* bulletTex);
};

#endif // ENEMYTANK_H
