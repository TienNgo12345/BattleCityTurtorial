#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    PlayerTank player;
    std::vector<Wall> walls;
    std::vector<EnemyTank> enemies;

    int score;
    std::vector<int> lastScores;
    bool scoreRecorded;
    bool gameOver;

    std::string playerName;

    TTF_Font* font;
    SDL_Texture* gameBgTexture;    // Ảnh nền giao chiến
    SDL_Texture* playerTexture;      // Ảnh xe tank người chơi
    SDL_Texture* enemyTexture;       // Ảnh xe tank địch
    SDL_Texture* bulletTexture;      // Ảnh đạn

    Game(const std::string &name);
    ~Game();

    void generateWalls();
    void spawnEnemy();
    void resetGame();
    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b);
    void handleEvents();
    void update();
    void renderScore();
    void renderGameOver();
    void render();
    void run();
};

#endif // GAME_H
