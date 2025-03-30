#include "EnemyTank.h"
#include <cstdlib>
#include <algorithm>

EnemyTank::EnemyTank(int x, int y)
    : speed(2)
{
    rect = { x, y, TILE_SIZE, TILE_SIZE };
    dx = 1; dy = 0;
}

void EnemyTank::move() {
    rect.x += dx * speed;
    rect.y += dy * speed;
    if(rect.x < 0 || rect.x > SCREEN_WIDTH - rect.w)
        dx = -dx;
    if(rect.y < 0 || rect.y > SCREEN_HEIGHT - rect.h)
        dy = -dy;
}

void EnemyTank::shoot() {
    int r = std::rand() % 4; // 0: lên, 1: phải, 2: xuống, 3: trái
    int dX = 0, dY = 0;
    if(r == 0) { dX = 0; dY = -1; }
    else if(r == 1) { dX = 1; dY = 0; }
    else if(r == 2) { dX = 0; dY = 1; }
    else if(r == 3) { dX = -1; dY = 0; }
    int bulletX = rect.x + rect.w/2 - 4;
    int bulletY = rect.y + rect.h/2 - 4;
    bullets.push_back(Bullet(bulletX, bulletY, dX, dY));
}

void EnemyTank::updateBullets() {
    for(auto &b : bullets)
        b.update();
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                [](Bullet& b){ return !b.active; }), bullets.end());
}

void EnemyTank::render(SDL_Renderer* renderer, SDL_Texture* tankTex, SDL_Texture* bulletTex) {
    if(tankTex)
        SDL_RenderCopy(renderer, tankTex, NULL, &rect);
    else {
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    for(auto &b : bullets)
        b.render(renderer, bulletTex);
}
