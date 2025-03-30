#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

// Kích thước cửa sổ và map
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;

// Hàm chuyển đổi direction sang góc xoay (đơn vị độ)
double getAngle(int direction) {
    switch(direction) {
        case 1: return 0.0;    // lên
        case 2: return 180.0;  // xuống
        case 3: return 270.0;  // trái
        case 4: return 90.0;   // phải
        default: return 0.0;
    }
}

//
// Lớp Wall: vẽ tường bằng hình chữ nhật màu nâu
//
class Wall {
public:
    SDL_Rect rect;
    SDL_Color color;
    Wall(int x, int y) {
        rect = { x, y, TILE_SIZE, TILE_SIZE };
        color = { 139, 69, 19, 255 };
    }
    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
};

//
// Lớp Bullet: dùng cho cả xe tank người chơi và địch
//
class Bullet {
public:
    SDL_Rect rect;
    int speed;
    int dirX, dirY;
    bool active;

    Bullet(int x, int y, int dX, int dY)
        : speed(10), dirX(dX), dirY(dY), active(true) {
        rect = { x, y, 8, 8 };
    }
    void update() {
        rect.x += dirX * speed;
        rect.y += dirY * speed;
        if(rect.x < 0 || rect.x > SCREEN_WIDTH || rect.y < 0 || rect.y > SCREEN_HEIGHT)
            active = false;
    }
    void render(SDL_Renderer* renderer, SDL_Texture* bulletTex) {
        if(bulletTex)
            SDL_RenderCopy(renderer, bulletTex, NULL, &rect);
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

//
// Lớp PlayerTank: xe tank người chơi (xoay theo hướng bắn)
//
class PlayerTank {
public:
    SDL_Rect rect;
    int speed;
    int direction; // 1: lên, 2: xuống, 3: trái, 4: phải; 0: đứng yên
    std::vector<Bullet> bullets;

    PlayerTank(int x, int y)
        : speed(5), direction(0) {
        rect = { x, y, TILE_SIZE, TILE_SIZE };
    }
    void move() {
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
    void shoot() {
        int dX = 0, dY = 0;
        if(direction == 1) dY = -1;
        else if(direction == 2) dY = 1;
        else if(direction == 3) dX = -1;
        else dX = 1; // nếu đứng yên, mặc định bắn sang phải
        int bulletX = rect.x + rect.w/2 - 4;
        int bulletY = rect.y + rect.h/2 - 4;
        bullets.push_back(Bullet(bulletX, bulletY, dX, dY));
    }
    void updateBullets() {
        for(auto &b : bullets)
            b.update();
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet& b){ return !b.active; }), bullets.end());
    }
    // Render xe tank với xoay theo hướng (sử dụng texture nếu có)
    void render(SDL_Renderer* renderer, SDL_Texture* tankTex, SDL_Texture* bulletTex) {
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
};

//
// Lớp EnemyTank: xe tank địch, di chuyển ngẫu nhiên, bắn đạn theo hướng ngẫu nhiên
//
class EnemyTank {
public:
    SDL_Rect rect;
    int speed;
    int dx, dy;
    std::vector<Bullet> bullets;

    EnemyTank(int x, int y)
        : speed(2) {
        rect = { x, y, TILE_SIZE, TILE_SIZE };
        dx = 1; dy = 0;
    }
    void move() {
        rect.x += dx * speed;
        rect.y += dy * speed;
        if(rect.x < 0 || rect.x > SCREEN_WIDTH - rect.w)
            dx = -dx;
        if(rect.y < 0 || rect.y > SCREEN_HEIGHT - rect.h)
            dy = -dy;
    }
    // Bắn đạn theo hướng ngẫu nhiên: 0: lên, 1: phải, 2: xuống, 3: trái
    void shoot() {
        int r = std::rand() % 4;
        int dX = 0, dY = 0;
        if(r == 0) { dX = 0; dY = -1; }
        else if(r == 1) { dX = 1; dY = 0; }
        else if(r == 2) { dX = 0; dY = 1; }
        else if(r == 3) { dX = -1; dY = 0; }
        int bulletX = rect.x + rect.w/2 - 4;
        int bulletY = rect.y + rect.h/2 - 4;
        bullets.push_back(Bullet(bulletX, bulletY, dX, dY));
    }
    void updateBullets() {
        for(auto &b : bullets)
            b.update();
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [](Bullet& b){ return !b.active; }), bullets.end());
    }
    // Render xe tank địch với texture nếu có
    void render(SDL_Renderer* renderer, SDL_Texture* tankTex, SDL_Texture* bulletTex) {
        if(tankTex)
            SDL_RenderCopy(renderer, tankTex, NULL, &rect);
        else {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
        for(auto &b : bullets)
            b.render(renderer, bulletTex);
    }
};

//
// Lớp Game: quản lý toàn bộ game, menu và giao chiến
//
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

    // Constructor nhận tên người chơi
    Game(const std::string &name)
        : player(SCREEN_WIDTH/2 - TILE_SIZE/2, SCREEN_HEIGHT - TILE_SIZE - 10),
          running(true), score(0), scoreRecorded(false), gameOver(false),
          playerName(name) {
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

    ~Game() {
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

    void generateWalls() {
        for(int i = 0; i < SCREEN_WIDTH; i += TILE_SIZE)
            walls.push_back(Wall(i, 0));
        walls.push_back(Wall(200, 200));
        walls.push_back(Wall(240, 200));
        walls.push_back(Wall(280, 200));
    }

    // Sinh thêm xe địch ngẫu nhiên phía trên màn hình
    void spawnEnemy() {
        int x = std::rand() % (SCREEN_WIDTH - TILE_SIZE);
        int y = std::rand() % 100;
        enemies.push_back(EnemyTank(x, y));
    }

    // Reset lại game khi nhấn P ở màn hình Game Over
    void resetGame() {
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

    bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
        return SDL_HasIntersection(&a, &b);
    }

    void handleEvents() {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                running = false;
            if(gameOver) {
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p)
                    resetGame();
                continue;
            }
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_SPACE)
                    player.shoot();
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }
        }
    }

    void update() {
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

        for(auto &enemy : enemies) {
            enemy.move();
            if(std::rand() % 100 < 2)
                enemy.shoot();
            enemy.updateBullets();
        }

        if(enemies.size() < 3 && std::rand() % 100 < 5)
            spawnEnemy();

        for(auto &bullet : player.bullets) {
            if(!bullet.active) continue;
            for(auto &wall : walls) {
                if(checkCollision(bullet.rect, wall.rect)) {
                    bullet.active = false;
                    break;
                }
            }
            if(bullet.active) {
                for(auto it = enemies.begin(); it != enemies.end(); ) {
                    if(checkCollision(bullet.rect, it->rect)) {
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

        for(auto &enemy : enemies) {
            for(auto &bullet : enemy.bullets) {
                for(auto &wall : walls) {
                    if(bullet.active && checkCollision(bullet.rect, wall.rect)) {
                        bullet.active = false;
                        break;
                    }
                }
                if(bullet.active && checkCollision(bullet.rect, player.rect)) {
                    bullet.active = false;
                    gameOver = true;
                }
            }
        }

        for(auto &enemy : enemies) {
            if(checkCollision(player.rect, enemy.rect)) {
                gameOver = true;
                break;
            }
        }

        if(gameOver && !scoreRecorded) {
            lastScores.push_back(score);
            if(lastScores.size() > 3)
                lastScores.erase(lastScores.begin());
            scoreRecorded = true;
        }
    }

    void renderScore() {
        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
        if(surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect dstRect = { 10, 10, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
    }

    void renderGameOver() {
        std::string gameOverText = "GAME OVER, " + playerName + "!";
        SDL_Color red = { 255, 0, 0, 255 };
        SDL_Surface* surface = TTF_RenderText_Solid(font, gameOverText.c_str(), red);
        if(surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect dstRect = { SCREEN_WIDTH/2 - surface->w/2, SCREEN_HEIGHT/2 - surface->h - 20, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        int startY = SCREEN_HEIGHT/2 + 10;
        for(size_t i = 0; i < lastScores.size(); i++) {
            std::string line = "Last Score " + std::to_string(i+1) + ": " + std::to_string(lastScores[i]);
            SDL_Color yellow = { 255, 255, 0, 255 };
            SDL_Surface* lineSurf = TTF_RenderText_Solid(font, line.c_str(), yellow);
            if(lineSurf) {
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
        if(playSurf) {
            SDL_Texture* playTex = SDL_CreateTextureFromSurface(renderer, playSurf);
            SDL_Rect dstRect = { SCREEN_WIDTH/2 - playSurf->w/2, startY, playSurf->w, playSurf->h };
            SDL_RenderCopy(renderer, playTex, NULL, &dstRect);
            SDL_DestroyTexture(playTex);
            SDL_FreeSurface(playSurf);
        }
    }

    void render() {
        // Vẽ ảnh nền giao chiến nếu có, nếu không vẽ nền đen
        if(gameBgTexture)
            SDL_RenderCopy(renderer, gameBgTexture, NULL, NULL);
        else {
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

    void run() {
        while(running) {
            handleEvents();
            update();
            render();
            SDL_Delay(16); // ~60 FPS
        }
    }
};

//
// Hàm showMenu(): hiển thị menu với ảnh nền từ "C:/Users/Admin/Downloads/nen.jpg"
//
bool showMenu() {
    SDL_Window* menuWindow = SDL_CreateWindow("Menu - Battle City", SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!menuWindow) return false;
    SDL_Renderer* menuRenderer = SDL_CreateRenderer(menuWindow, -1, SDL_RENDERER_ACCELERATED);
    if(!menuRenderer) {
        SDL_DestroyWindow(menuWindow);
        return false;
    }
    SDL_Texture* bgTexture = IMG_LoadTexture(menuRenderer, "C:/Users/Admin/Downloads/nen.jpg");
    if (!bgTexture) {
        std::cout << "Không tải được ảnh nền menu! Error: " << IMG_GetError() << std::endl;
    }
    TTF_Font* menuFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 36);
    if(!menuFont) {
        if(bgTexture) SDL_DestroyTexture(bgTexture);
        SDL_DestroyRenderer(menuRenderer);
        SDL_DestroyWindow(menuWindow);
        return false;
    }
    bool menuRunning = true;
    bool startGame = false;
    while(menuRunning) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                menuRunning = false;
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_RETURN) {
                    startGame = true;
                    menuRunning = false;
                }
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    menuRunning = false;
                }
            }
        }
        if(bgTexture)
            SDL_RenderCopy(menuRenderer, bgTexture, NULL, NULL);
        else {
            SDL_SetRenderDrawColor(menuRenderer, 0, 0, 0, 255);
            SDL_RenderClear(menuRenderer);
        }
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* titleSurf = TTF_RenderText_Solid(menuFont, "Battle City", white);
        SDL_Texture* titleTex = SDL_CreateTextureFromSurface(menuRenderer, titleSurf);
        SDL_Rect titleRect = { SCREEN_WIDTH/2 - titleSurf->w/2, 100, titleSurf->w, titleSurf->h };
        SDL_RenderCopy(menuRenderer, titleTex, NULL, &titleRect);
        SDL_FreeSurface(titleSurf);
        SDL_DestroyTexture(titleTex);
        SDL_Surface* playSurf = TTF_RenderText_Solid(menuFont, "Press Enter to Play", white);
        SDL_Texture* playTex = SDL_CreateTextureFromSurface(menuRenderer, playSurf);
        SDL_Rect playRect = { SCREEN_WIDTH/2 - playSurf->w/2, 250, playSurf->w, playSurf->h };
        SDL_RenderCopy(menuRenderer, playTex, NULL, &playRect);
        SDL_FreeSurface(playSurf);
        SDL_DestroyTexture(playTex);
        SDL_Surface* exitSurf = TTF_RenderText_Solid(menuFont, "Press Esc to Exit", white);
        SDL_Texture* exitTex = SDL_CreateTextureFromSurface(menuRenderer, exitSurf);
        SDL_Rect exitRect = { SCREEN_WIDTH/2 - exitSurf->w/2, 350, exitSurf->w, exitSurf->h };
        SDL_RenderCopy(menuRenderer, exitTex, NULL, &exitRect);
        SDL_FreeSurface(exitSurf);
        SDL_DestroyTexture(exitTex);
        SDL_RenderPresent(menuRenderer);
        SDL_Delay(16);
    }
    TTF_CloseFont(menuFont);
    if(bgTexture) SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(menuRenderer);
    SDL_DestroyWindow(menuWindow);
    return startGame;
}

//
// Hàm main: nhập tên, hiển thị menu, sau đó chạy game nếu người chơi chọn Play
//
int main(int argc, char* argv[]) {
    if(IMG_Init(IMG_INIT_JPG) != IMG_INIT_JPG) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return 1;
    }
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if(TTF_Init() == -1) {
        std::cout << "TTF Init Error: " << TTF_GetError() << std::endl;
        return 1;
    }
    std::string name;
    std::cout << "Enter Your Name: ";
    std::getline(std::cin, name);
    bool play = showMenu();
    if(!play) {
        TTF_Quit();
        SDL_Quit();
        IMG_Quit();
        return 0;
    }
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
    Game game(name);
    if(game.running)
        game.run();
    return 0;
}
