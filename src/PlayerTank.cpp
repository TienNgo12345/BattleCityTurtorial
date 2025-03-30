#include "PlayerTank.h"
#include <algorithm>

double getAngle(int direction) {
    switch(direction) {
        case 1: return 0.0;    // lên
        case 2: return 180.0;  // xuống
        case 3: return 270.0;  // trái
        case 4: return 90.0;   // phải
        default: return 0.0;
    }
}

PlayerTank::PlayerTank(int x, int y)
    : speed(5), direction(0)
{
    rect = { x, y, TILE_SIZE, TILE_SIZE };
}

void PlayerTank::move() {
    switch(direction) {
        case 1: rect.y -= speed; break;
        case 2: rect.y += speed; break;
        case 3: rect.x -= speed; break;
        case 4: rect.x += speed; break;
        default: break;
    }
    if(rect.x < 0) rect.x = 0;
    if(rect.y < 0) rect.y = 0;
    if(rect.x > SCREEN_WIDTH - rect.w) rect.x = SCREEN_WIDTH - rect.w;
    if(rect.y > SCREEN_HEIGHT - rect.h) rect.y = SCREEN_HEIGHT - rect.h;
}

void PlayerTank::shoot() {
    int dX = 0, dY = 0;
    if(direction == 1) dY = -1;
    else if(direction == 2) dY = 1;
    else if(direction == 3) dX = -1;
    else dX = 1; // nếu đứng yên, mặc định bắn sang phải
    int bulletX = rect.x + rect.w/2 - 4;
    int bulletY = rect.y + rect.h/2 - 4;
    bullets.push_back(Bullet(bulletX, bulletY, dX, dY));
}

void PlayerTank::updateBullets() {
    for(auto &b : bullets)
        b.update();
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                [](Bullet& b){ return !b.active; }), bullets.end());
}

void PlayerTank::render(SDL_Renderer* renderer, SDL_Texture* tankTex, SDL_Texture* bulletTex) {
    double angle = getAngle(direction);
    if(tankTex)
        SDL_RenderCopyEx(renderer, tankTex, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
    else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    for(auto &b : bullets)
        b.render(renderer, bulletTex);
}
