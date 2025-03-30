#include "Wall.h"

Wall::Wall(int x, int y) {
    rect = { x, y, TILE_SIZE, TILE_SIZE };
    color = { 139, 69, 19, 255 };
}

void Wall::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}
