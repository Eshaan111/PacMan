#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define TILE_SIZE (SCREEN_WIDTH / 20)  // 54 pixels per tile
#define MAP_WIDTH (SCREEN_WIDTH / TILE_SIZE)   // 20 columns
#define MAP_HEIGHT (SCREEN_HEIGHT / TILE_SIZE) // 13 rows
#define PACMAN_SIZE 40
#define DECAY_RATE 0.85f
#define SPEED 200.0f

typedef struct {
    float x, y;
    float vx, vy;
} Entity;

typedef struct {
    Entity pacman;
    SDL_Texture *pacman_texture;
    SDL_Texture *wall_texture;
} GameState;

int level1[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,1,1,0,1,0,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {0,0,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int curr_level[13][20];
int curr_level_num = 1;
int prev_level_num =0;
void array_copy(int curr_level[13][20], int level_array[13][20]){
    for(int i=0;i<13;i++){
        for(int j=0;j<20;j++){
            curr_level[i][j] = level_array[i][j];
        }
    }
}

// void set_level(GameState *game){
//     float x_pos = game->pacman.x;
//     float y_pos = game->pacman.x;

    
//     switch (expression)
//     {
//     case constant expression:
//         /* code */
//         break;
    
//     default:
//         break;
//     }

// }

// Find an empty tile and return its position
void SpawnPacman(float *x, float *y,int curr_level) {
    switch (curr_level)
    {
    case 1:
        *x = TILE_SIZE*1 + (TILE_SIZE-PACMAN_SIZE)/2;
        *y = TILE_SIZE*1 + (TILE_SIZE-PACMAN_SIZE)/2;
        break;
    case 2:
        *x = TILE_SIZE*0 + (TILE_SIZE-PACMAN_SIZE)/2;
        *y = TILE_SIZE*6 + (TILE_SIZE-PACMAN_SIZE)/2;
        break;
    default:
        break;
    }
    
    
    // for (int row = 0; row < MAP_HEIGHT; row++) {
    //     for (int col = 0; col < MAP_WIDTH; col++) {
    //         if (curr_level[row][col] == 0) {  // Found empty space
    //             *x = col * TILE_SIZE + (TILE_SIZE - PACMAN_SIZE) / 2;  // Center in tile
    //             *y = row * TILE_SIZE + (TILE_SIZE - PACMAN_SIZE) / 2;
    //             return;
    //         }
    //     }
    // }
}

// Collision detection with all four corners of Pac-Man
int checkCollision(float x, float y) {
    int tileX1 = (int)(x / TILE_SIZE); //left wall
    int tileY1 = (int)(y / TILE_SIZE); //top wall
    int tileX2 = (int)((x + PACMAN_SIZE - 1) / TILE_SIZE); //right wall
    int tileY2 = (int)((y + PACMAN_SIZE - 1) / TILE_SIZE); //bottom wall
    //LEAVE LEVEL CASE :
    // if(tileY1 ==6 && tileX1 == 19){
    //     SDL_Quit();
    // }

    //OUT OF BOUNDS
    if (tileX1 < 0 || tileX2 >= MAP_WIDTH || tileY1 < 0 || tileY2 >= MAP_HEIGHT)
        return 1;
    //WALL DETECTED
    if (curr_level[tileY1][tileX1] == 1 || curr_level[tileY1][tileX2] == 1 ||
        curr_level[tileY2][tileX1] == 1 || curr_level[tileY2][tileX2] == 1) {
        return 1;
    }

    return 0;

}

int check_level_end(float x,float y){
    int tileX1 = (int)(x / TILE_SIZE); //left wall
    int tileY1 = (int)(y / TILE_SIZE); //top wall

    switch (curr_level_num)
    {
    case 1:
    if(tileY1 ==6 && tileX1 == 19){
        curr_level_num = 2;
        return 1;
        //SDL_Quit();
    }break;
    
    default:
        break;
    }

    return 0;
}

int check_events(SDL_Window *window, GameState *game, float speed, float deltaTime) {
    int quit = 0;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            quit = 1;
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    float acceleration = speed * deltaTime;
    float newX = game->pacman.x;
    float newY = game->pacman.y;

    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        newX -= acceleration;
    else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        newX += acceleration;
    else
        game->pacman.vx *= DECAY_RATE;

    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        newY -= acceleration;
    else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        newY += acceleration;
    else
        game->pacman.vy *= DECAY_RATE;

    if (!checkCollision(newX, game->pacman.y))
        game->pacman.x = newX;
    if (!checkCollision(game->pacman.x, newY))
        game->pacman.y = newY;

    check_level_end(game->pacman.x,game->pacman.y);
    return quit;
}

void renderGame(SDL_Renderer *renderer, GameState *game) {
    SDL_SetRenderDrawColor(renderer, 26,26,26, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,204,204,204, 255);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (curr_level[y][x] == 1) {
                SDL_Rect wallRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, game->wall_texture, NULL, &wallRect);
            }
        }
    }

    SDL_Rect pacmanRect = { (int)game->pacman.x, (int)game->pacman.y, PACMAN_SIZE, PACMAN_SIZE };
    SDL_RenderCopy(renderer, game->pacman_texture, NULL, &pacmanRect);


    SDL_RenderPresent(renderer);
}

int main() {
    

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Pac-Man Clone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!window || !renderer) {
        printf("Failed to create window or renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *pacmanSurface = IMG_Load("pacman-png-25200.png");
    if (!pacmanSurface) {
        printf("Could not load Pac-Man image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Surface *wallSurface = IMG_Load("moss.png");
    if (!wallSurface) {
        printf("Could not load Wall image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }



    GameState game;
    SpawnPacman(&game.pacman.x, &game.pacman.y,curr_level_num);
    game.pacman.vx = 0;
    game.pacman.vy = 0;
    game.pacman_texture = SDL_CreateTextureFromSurface(renderer, pacmanSurface);
    game.wall_texture = SDL_CreateTextureFromSurface(renderer,wallSurface);
    SDL_FreeSurface(pacmanSurface);
    SDL_FreeSurface(wallSurface);

    Uint32 lastTime = SDL_GetTicks();
    int quit = 0;

    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
    
        // Change level only when needed
        if (curr_level_num != prev_level_num) {
            if (curr_level_num == 1) {
                array_copy(curr_level, level1);
            } else if (curr_level_num == 2) {
                array_copy(curr_level, level2);
            }
            
            // Respawn Pac-Man when switching levels
            SpawnPacman(&game.pacman.x, &game.pacman.y,curr_level_num);
            game.pacman.vx = 0;
            game.pacman.vy = 0;
    
            prev_level_num = curr_level_num;  // Update previous level tracker
        }
    
        quit = check_events(window, &game, SPEED, deltaTime);
        renderGame(renderer, &game);
    }
    SDL_DestroyTexture(game.pacman_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
