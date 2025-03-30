#include "Utilities.h"

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    if (a.x + a.w <= b.x || a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y || a.y >= b.y + b.h) return false;
    return true;
}
