#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include <SDL.h>
#include <vector>
#include "Bullet.h"
#include "constants.h"

// Hàm chuyển đổi direction sang góc xoay (đơn vị độ)
double getAngle(int direction);

class PlayerTank {
public:
    SDL_Rect rect;
    int speed;
    int direction; // 1: lên, 2: xuống, 3: trái, 4: phải; 0: đứng yên
    std::vector<Bullet> bullets;

    PlayerTank(int x, int y);
    void move();
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer, SDL_Texture* tankTex, SDL_Texture* bulletTex);
};

#endif // PLAYERTANK_H
