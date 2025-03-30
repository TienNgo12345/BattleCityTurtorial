#include "Bullet.h"

Bullet::Bullet(int x, int y, int dX, int dY)
    : speed(10), dirX(dX), dirY(dY), active(true)
{
    rect = { x, y, 8, 8 };
}

void Bullet::update() {
    rect.x += dirX * speed;
    rect.y += dirY * speed;
    if(rect.x < 0 || rect.x > SCREEN_WIDTH || rect.y < 0 || rect.y > SCREEN_HEIGHT)
        active = false;
}

void Bullet::render(SDL_Renderer* renderer, SDL_Texture* bulletTex) {
    if(bulletTex)
        SDL_RenderCopy(renderer, bulletTex, NULL, &rect);
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
