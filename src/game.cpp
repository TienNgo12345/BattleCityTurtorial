#include "Game.h"
#include "constants.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game(const std::string &name)
    : player(SCREEN_WIDTH/2 - TILE_SIZE/2, SCREEN_HEIGHT - TILE_SIZE - 10),
      running(true), score(0), scoreRecorded(false), gameOver(false),
      playerName(name)
{
    std::srand((unsigned int)std::time(nullptr));
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Init Error: " << SDL_GetError() << std::endl;
        running = false;
    }
    if(TTF_Init() == -1) {
        std::cout << "TTF Init Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!window) {
        std::cout << "Window Error: " << SDL_GetError() << std::endl;
        running = false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        std::cout << "Renderer Error: " << SDL_GetError() << std::endl;
        running = false;
    }
    font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if(!font) {
        std::cout << "Font Error: " << TTF_GetError() << std::endl;
        running = false;
    }
    // Tải ảnh nền giao chiến
    gameBgTexture = IMG_LoadTexture(renderer, "C:/Users/Admin/Downloads/nengame.jpg");
    if(!gameBgTexture)
        std::cout << "Không tải được ảnh nền giao chiến! Error: " << IMG_GetError() << std::endl;
    // Tải ảnh cho xe tank người chơi
    playerTexture = IMG_LoadTexture(renderer, "C:/Users/Admin/Downloads/tankcuat.jpg");
    if(!playerTexture)
        std::cout << "Không tải được ảnh xe tank người chơi! Error: " << IMG_GetError() << std::endl;
    // Tải ảnh cho xe tank địch
    enemyTexture = IMG_LoadTexture(renderer, "C:/Users/Admin/Downloads/tankdich.jpg");
    if(!enemyTexture)
        std::cout << "Không tải được ảnh xe tank địch! Error: " << IMG_GetError() << std::endl;
    // Tải ảnh cho đạn
    bulletTexture = IMG_LoadTexture(renderer, "C:/Users/Admin/Downloads/bullet.jpg");
    if(!bulletTexture)
        std::cout << "Không tải được ảnh đạn! Error: " << IMG_GetError() << std::endl;

    generateWalls();
    enemies.push_back(EnemyTank(50, 50));
    enemies.push_back(EnemyTank(300, 50));
    enemies.push_back(EnemyTank(550, 50));
}

Game::~Game() {
    if(gameBgTexture) SDL_DestroyTexture(gameBgTexture);
    if(playerTexture) SDL_DestroyTexture(playerTexture);
    if(enemyTexture) SDL_DestroyTexture(enemyTexture);
    if(bulletTexture) SDL_DestroyTexture(bulletTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::generateWalls(){
    for(int i = 0; i < SCREEN_WIDTH; i += TILE_SIZE)
        walls.push_back(Wall(i, 0));
    walls.push_back(Wall(200, 200));
    walls.push_back(Wall(240, 200));
    walls.push_back(Wall(280, 200));
}

void Game::spawnEnemy(){
    int x = std::rand() % (SCREEN_WIDTH - TILE_SIZE);
    int y = std::rand() % 100;
    enemies.push_back(EnemyTank(x, y));
}

void Game::resetGame(){
    score = 0;
    player.rect.x = SCREEN_WIDTH/2 - TILE_SIZE/2;
    player.rect.y = SCREEN_HEIGHT - TILE_SIZE - 10;
    enemies.clear();
    enemies.push_back(EnemyTank(50, 50));
    enemies.push_back(EnemyTank(300, 50));
    enemies.push_back(EnemyTank(550, 50));
    gameOver = false;
    scoreRecorded = false;
}

bool Game::checkCollision(const SDL_Rect& a, const SDL_Rect& b){
    return SDL_HasIntersection(&a, &b);
}

void Game::handleEvents(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT)
            running = false;
        if(gameOver){
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p)
                resetGame();
            continue;
        }
        if(event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_SPACE)
                player.shoot();
            if(event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
        }
    }
}

void Game::update(){
    if(gameOver)
        return;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    int hor = 0, ver = 0;
    if(keystate[SDL_SCANCODE_UP])
        ver = -1;
    if(keystate[SDL_SCANCODE_DOWN])
        ver = 1;
    if(keystate[SDL_SCANCODE_LEFT])
        hor = -1;
    if(keystate[SDL_SCANCODE_RIGHT])
        hor = 1;
    if(ver != 0)
        player.direction = (ver < 0) ? 1 : 2;
    else if(hor != 0)
        player.direction = (hor < 0) ? 3 : 4;
    else
        player.direction = 0;

    player.move();
    player.updateBullets();

    for(auto &enemy : enemies){
        enemy.move();
        if(std::rand() % 100 < 2)
            enemy.shoot();
        enemy.updateBullets();
    }

    if(enemies.size() < 3 && std::rand() % 100 < 5)
        spawnEnemy();

    for(auto &bullet : player.bullets){
        if(!bullet.active) continue;
        for(auto &wall : walls){
            if(checkCollision(bullet.rect, wall.rect)){
                bullet.active = false;
                break;
            }
        }
        if(bullet.active){
            for(auto it = enemies.begin(); it != enemies.end(); ){
                if(checkCollision(bullet.rect, it->rect)){
                    bullet.active = false;
                    it = enemies.erase(it);
                    score++;
                    spawnEnemy();
                    break;
                } else {
                    ++it;
                }
            }
        }
    }

    for(auto &enemy : enemies){
        for(auto &bullet : enemy.bullets){
            for(auto &wall : walls){
                if(bullet.active && checkCollision(bullet.rect, wall.rect)){
                    bullet.active = false;
                    break;
                }
            }
            if(bullet.active && checkCollision(bullet.rect, player.rect)){
                bullet.active = false;
                gameOver = true;
            }
        }
    }

    for(auto &enemy : enemies){
        if(checkCollision(player.rect, enemy.rect)){
            gameOver = true;
            break;
        }
    }

    if(gameOver && !scoreRecorded){
        lastScores.push_back(score);
        if(lastScores.size() > 3)
            lastScores.erase(lastScores.begin());
        scoreRecorded = true;
    }
}

void Game::renderScore(){
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
    if(surface){
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstRect = { 10, 10, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}

void Game::renderGameOver(){
    std::string gameOverText = "GAME OVER, " + playerName + "!";
    SDL_Color red = { 255, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, gameOverText.c_str(), red);
    if(surface){
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstRect = { SCREEN_WIDTH/2 - surface->w/2, SCREEN_HEIGHT/2 - surface->h - 20, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
    int startY = SCREEN_HEIGHT/2 + 10;
    for(size_t i = 0; i < lastScores.size(); i++){
        std::string line = "Last Score " + std::to_string(i+1) + ": " + std::to_string(lastScores[i]);
        SDL_Color yellow = { 255, 255, 0, 255 };
        SDL_Surface* lineSurf = TTF_RenderText_Solid(font, line.c_str(), yellow);
        if(lineSurf){
            SDL_Texture* lineTex = SDL_CreateTextureFromSurface(renderer, lineSurf);
            SDL_Rect dstRect = { SCREEN_WIDTH/2 - lineSurf->w/2, startY, lineSurf->w, lineSurf->h };
            SDL_RenderCopy(renderer, lineTex, NULL, &dstRect);
            SDL_DestroyTexture(lineTex);
            SDL_FreeSurface(lineSurf);
            startY += 30;
        }
    }
    std::string playAgain = "Press P to Play Again";
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* playSurf = TTF_RenderText_Solid(font, playAgain.c_str(), white);
    if(playSurf){
        SDL_Texture* playTex = SDL_CreateTextureFromSurface(renderer, playSurf);
        SDL_Rect dstRect = { SCREEN_WIDTH/2 - playSurf->w/2, startY, playSurf->w, playSurf->h };
        SDL_RenderCopy(renderer, playTex, NULL, &dstRect);
        SDL_DestroyTexture(playTex);
        SDL_FreeSurface(playSurf);
    }
}

void Game::render(){
    if(gameBgTexture)
        SDL_RenderCopy(renderer, gameBgTexture, NULL, NULL);
    else{
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
    for(auto &wall : walls)
        wall.render(renderer);
    player.render(renderer, playerTexture, bulletTexture);
    for(auto &enemy : enemies)
        enemy.render(renderer, enemyTexture, bulletTexture);
    renderScore();
    if(gameOver)
        renderGameOver();
    SDL_RenderPresent(renderer);
}

void Game::run(){
    while(running){
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}
