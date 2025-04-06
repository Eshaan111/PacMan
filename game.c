#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define TILE_SIZE (SCREEN_WIDTH / 20)          // 54 pixels per tile
#define MAP_WIDTH (SCREEN_WIDTH / TILE_SIZE)   // 20 columns
#define MAP_HEIGHT (SCREEN_HEIGHT / TILE_SIZE) // 13 rows
#define PACMAN_SIZE 40
#define DECAY_RATE 0.85f
#define SPEED 200.0f

typedef struct
{
    float x, y;
    float vx, vy;
} Entity;

typedef struct
{
    Entity pacman;
    Entity ghost;
    Entity arrow_up;
    Entity arrow_down;
    SDL_Texture *pacman_up_texture;
    SDL_Texture *pacman_front_texture;
    SDL_Texture *pacman_down_texture;
    SDL_Texture *pacman_back_texture;
    SDL_Texture *pacman_up_shield_texture;
    SDL_Texture *pacman_front_shield_texture;
    SDL_Texture *pacman_down_shield_texture;
    SDL_Texture *pacman_back_shield_texture;
    SDL_Texture *ghost_texture;
    SDL_Texture *arrow_up_texture;
    SDL_Texture *arrow_down_texture;
    SDL_Texture *moss_texture;
    SDL_Texture *stone_texture;
    SDL_Texture *water_texture;
    SDL_Texture *lava_texture;
    SDL_Texture *lava_flow_texture;
    SDL_Texture *water_flow_texture;
    SDL_Texture *game_over_texture;
    SDL_Texture *play_again_texture;
    SDL_Texture *quit_texture;

} GameState;

int level1[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 ,1},
    {1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}
};

int level3[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int level4[MAP_HEIGHT][MAP_WIDTH] = {
    //  (1,0)
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}
};
int level5[MAP_HEIGHT][MAP_WIDTH] = {
    //  (1,0)
    {2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

int level5[MAP_HEIGHT][MAP_WIDTH] = {
    //  (1,0)
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2},
    {2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2},
    {2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2},
    {2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};


int curr_level[13][20];
int curr_level_num = 5;
int prev_level_num = 0;
int shield = 0;
SDL_Texture *curr_pacman_texture;
SDL_Texture *curr_shield_texture;
Uint32 lastSpacePressTime = 0;
Uint32 currentTime = 0;
Uint32 spacePressStartTime = 0;
Uint32 cooldown_start=0;

void array_copy(int curr_level[13][20], int level_array[13][20])
{
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            curr_level[i][j] = level_array[i][j];
        }
    }
}

void set_pacman_spawn(float *x, float *y, int curr_level)
{
    switch (curr_level)
    {
    case 1:
        *x = TILE_SIZE * 1 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 1 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;
    case 2:
        *x = TILE_SIZE * 0 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 6 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 3:
        *x = TILE_SIZE * 1 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 0 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 4:
        *x = TILE_SIZE * 0 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;
    case 5 :
        *x = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 6 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    
    case 5:
        *x = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 8 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;
    default:
        break;
    }
}

void set_ghost_spawn(float *x, float *y, int curr_level,GameState *game)
{
    switch (curr_level)
    {
    case 1:
        *x =0;
        *y = 0;
        break;
    case 2:
        *x =0;
        *y = 0;
        break;
    case 3:
        *x = TILE_SIZE * 10 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;
    case 4:
        *x = TILE_SIZE * 8+ (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 5 + (TILE_SIZE - PACMAN_SIZE) / 2;
<<<<<<< HEAD
        break;
    case 5 :
        *x = game->pacman.x + (0.95*TILE_SIZE);
        *y = game->pacman.y;
        break;    
=======
        break; 

     case 5:
        *x = TILE_SIZE * 10+ (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 8+ (TILE_SIZE - PACMAN_SIZE) / 2;
        break;       
>>>>>>> 722dce316958a92ce6fdcf7144452bbf6fda1cde
    }
}

// Collision detection with all four corners of Pac-Man
int checkCollision(float x, float y)
{
    int tileX1 = (int)(x / TILE_SIZE);                     // left wall
    int tileY1 = (int)(y / TILE_SIZE);                     // top wall
    int tileX2 = (int)((x + PACMAN_SIZE - 1) / TILE_SIZE); // right wall
    int tileY2 = (int)((y + PACMAN_SIZE - 1) / TILE_SIZE); // bottom wall
    // LEAVE LEVEL CASE :
    //  if(tileY1 ==6 && tileX1 == 19){
    //      SDL_Quit();
    //  }

    // OUT OF BOUNDS
    if (tileX1 < 0 || tileX2 >= MAP_WIDTH || tileY1 < 0 || tileY2 >= MAP_HEIGHT)
        return 1;
    // WALL DETECTED
    if (curr_level[tileY1][tileX1] == 1 || curr_level[tileY1][tileX2] == 1 ||
        curr_level[tileY2][tileX1] == 1 || curr_level[tileY2][tileX2] == 1)
    {
        return 1;
    }
    if (curr_level[tileY1][tileX1] == 2 || curr_level[tileY1][tileX2] == 2 ||
        curr_level[tileY2][tileX1] == 2 || curr_level[tileY2][tileX2] == 2)
    {
        return 1;
    }
    if (curr_level[tileY1][tileX1] == 3 || curr_level[tileY1][tileX2] == 3 ||
        curr_level[tileY2][tileX1] == 3 || curr_level[tileY2][tileX2] == 3)
    {
        return 1;
    }

    return 0;
}

void moveGhost(GameState *game, float deltaTime)
{
    float speed = 100.0f; // Increased speed
    if(curr_level_num == 5 || curr_level_num == 1 || curr_level_num == 2){
        speed = 0;
    }
    float dx = game->pacman.x - game->ghost.x;
    float dy = game->pacman.y - game->ghost.y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        game->ghost.vx = (dx / distance) * speed * deltaTime;
        game->ghost.vy = (dy / distance) * speed * deltaTime;
    }

    float newX = game->ghost.x + game->ghost.vx;
    float newY = game->ghost.y + game->ghost.vy;

    // First try moving in X direction
    if (!checkCollision(newX, game->ghost.y))
    {
        game->ghost.x = newX;
    }

    // Then try moving in Y direction
    if (!checkCollision(game->ghost.x, newY))
    {
        game->ghost.y = newY;
    }
}

SDL_Texture *curr_primary_text(int curr_level, GameState *game)
{
    switch (curr_level)
    {
    case 1:
        return game->moss_texture;
        break;

    case 2:
        return game->water_texture;
        break;
    case 3:
        return game->stone_texture;
        break;
    case 4:
        return game->lava_texture;
        break;
<<<<<<< HEAD
    case 5:
        return game ->lava_texture;
        break;
    
=======
    
    case 5:
        return game->lava_texture;
        break;
>>>>>>> 722dce316958a92ce6fdcf7144452bbf6fda1cde
    }
}

SDL_Texture *curr_second_text(int curr_level, GameState *game)
{
    switch (curr_level)
    {
    case 1:
        return game->moss_texture;
        break;

    case 2:
        return game->water_flow_texture;
        break;
    case 3:
        return game->stone_texture;
        break;

    case 4:
        return game->lava_flow_texture;
        break;
    case 5:
<<<<<<< HEAD
        return game ->stone_texture;
=======
        return game->stone_texture;
>>>>>>> 722dce316958a92ce6fdcf7144452bbf6fda1cde
        break;
    }
}

void arrow_logic(GameState *game, int level)
{
    if (level == 1 || level == 3 || level == 5)
    {
        game->arrow_up.x = 0;
        game->arrow_down.x = 0;
    }
    else
    {
        int arrow_speed = 5;
        // int y =game->pacman.y;
        // int y1 = y/TILE_SIZE;
        // int y2 = ((12*TILE_SIZE)-y)/TILE_SIZE;
        // printf("--%d %d--",y1+1,y2);
        // if(y1+1 > 3 && y2> 3){
        //     arrow_speed = 5; // Speed of arrows in pixels per frame
        // }
        // else{
        //     arrow_speed = 0;
        // }
        // Ensure arrows spawn in valid locations
        if (game->arrow_up.x == -1)
        {   
            int x1 = (rand() % (SCREEN_WIDTH - 3 * TILE_SIZE)) + (3 * TILE_SIZE);
            game->arrow_up.x = x1 + (TILE_SIZE/2);
            game->arrow_up.y = 11 * TILE_SIZE;
            
        }

        if (game->arrow_down.x == -1)
        { 
            int x2 = (rand() % (SCREEN_WIDTH - 3 * TILE_SIZE)) + (3 * TILE_SIZE);
            game->arrow_down.x = (int)x2 + (TILE_SIZE/2);
            game->arrow_down.y = 0;
           
        }

        // Move arrows
        game->arrow_up.y -= arrow_speed;
        game->arrow_down.y += arrow_speed;

        // Reset arrows when they go off screen
        if (game->arrow_up.y <= 0)
        {
            game->arrow_up.y = -1;
            game->arrow_up.x = -1;
            
        }
        if (game->arrow_down.y >= 11 * TILE_SIZE)
        {
            game->arrow_down.y = -1;
            game->arrow_down.x = -1;
            
        }
    }
}

int check_level_end(float x, float y, GameState *game)
{

    int tileX1 = (int)(x / TILE_SIZE); // left wall
    int tileY1 = (int)(y / TILE_SIZE); // top wall

    switch (curr_level_num)
    {
    case 1:
        if (tileY1 == 6 && tileX1 == 19)
        {   
            printf("Ending level 1");
            curr_level_num = 2;
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num,game);
            game->arrow_up.x = -1;
            game->arrow_down.x = -1;
            return 1;
            // SDL_Quit();
        }
        break;
    case 2:
        if (tileY1 == 11 && tileX1 == 18)
        {
            printf("Ending level 2");
            curr_level_num = 3;
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num,game);
            game->arrow_up.x = 0;
            game->arrow_down.x = 0;
            return 1;
        // SDL_Quit();
        
        }break;
    case 3:
        if (tileY1 == 8 && tileX1 == 19)
        {
            printf("Ending level 3");
            curr_level_num = 4;
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num,game);
            game->arrow_up.x = -1;
            game->arrow_down.x = -1;
            return 1;
            // SDL_Quit();
        }
        break;
    case 4:
         if (tileY1 == 1 && tileX1 == 1){
            level4[1][2]=0;
            level4[1][3]=0;
            level4[1][4]=0;
         }
         
         if(tileY1 == 1 && tileX1 ==5 ){
            level4[1][2]=1;
            level4[1][3]=1;
            level4[1][4]=1;

         }
         if(tileY1 == 1 && tileX1 ==9){
            level4[1][10] =0;
            level4[2][9] = 0;
            level4[3][9] = 0;
            level4[4][9] = 0;

            level4[1][13] =1;
            level4[3][8] =1;
            level4[3][10] =1;

         }
         if(tileY1 ==5 && tileX1 ==9){
            level4[1][10] =0;
            level4[2][9] = 1;
            level4[3][9] = 2;
            level4[4][9] = 1;

            level4[1][13] =0;
            level4[3][8] =2;
            level4[3][10] =2;
         }
         if(tileY1 == 11 && tileX1 == 18){
            printf("Ending level ");
            curr_level_num = 5;
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num,game);
         }
         array_copy(curr_level,level4);
<<<<<<< HEAD
        break;
    
    case 5:
        set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num,game);
        if(tileY1 == 8 && tileX1 ==1){
            curr_level_num = 1;
        }
        else if(tileY1 == 8 && tileX1 ==2){
            curr_level_num = 1;
        }
        else if(tileY1 == 8 && tileX1 ==3){
            curr_level_num = 1;
        }
        else if(tileY1 == 8 && tileX1 ==17){
            curr_level_num = 1;
            SDL_Quit();
        }
        else if(tileY1 == 8 && tileX1 ==16){
            
            SDL_Quit();
        }
        
        break;    
    
=======
        if (tileY1 == 11 && tileX1 == 19)
        {
            curr_level_num = 4;
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num);
>>>>>>> 722dce316958a92ce6fdcf7144452bbf6fda1cde
    default:
        break;
    }

    return 0;
}
}

int check_events(SDL_Window *window, GameState *game, float speed, float deltaTime)
{   
    
    int quit = 0;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
        {
            quit = 1;
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    float acceleration = speed * deltaTime;
    float newX = game->pacman.x;
    float newY = game->pacman.y;

    //movement detection
     if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    {
        newX -= acceleration;
        curr_pacman_texture = game->pacman_back_texture;
        curr_shield_texture = game->pacman_back_shield_texture;
    }
    else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    {
        newX += acceleration;
        curr_pacman_texture = game->pacman_front_texture;
        curr_shield_texture = game->pacman_front_shield_texture;
    }
    else
        game->pacman.vx *= DECAY_RATE;

    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
        newY -= acceleration;
        curr_pacman_texture = game->pacman_up_texture;
        curr_shield_texture = game->pacman_up_shield_texture;
    }
    else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
        newY += acceleration;
        curr_pacman_texture = game->pacman_down_texture;
        curr_shield_texture = game->pacman_down_shield_texture;
    }
    else
        game->pacman.vy *= DECAY_RATE;

    //detect spacebar for arrow deflection
    //  if (keys[SDL_SCANCODE_SPACE]) {
    //     lastSpacePressTime = currentTime; // Update last press time
    // }
        
    if (keys[SDL_SCANCODE_SPACE]) {
        if (currentTime - cooldown_start > 3000) {  // Apply 3-second cooldown
            if (spacePressStartTime == 0) {
                spacePressStartTime = currentTime;  // First press detected
                cooldown_start = currentTime;  // Start cooldown
            }
            lastSpacePressTime = currentTime;  // Reset shield duration timer

            // If space has been held continuously for 1 second, disable shield
            if (currentTime - spacePressStartTime >= 1000) {
                shield = 0;
            } else {
                shield = 1;
            }
        }
    } 
    else {
        spacePressStartTime = 0;  // Reset hold timer when space is released
    }

    // If space was **tapped**, keep the shield for 1 second after last press
    if (currentTime - lastSpacePressTime >= 1000) {
        shield = 0;  // Shield expires 1 sec after last tap
    }

    if (!checkCollision(newX, game->pacman.y))
        game->pacman.x = newX;
    if (!checkCollision(game->pacman.x, newY))
    {
        game->pacman.y = newY;
    }
    
    //GHOST PACMAN COLLISION 
        if(curr_level_num!=5){
            if (fabs(game->pacman.x - game->ghost.x) < PACMAN_SIZE && fabs(game->pacman.y - game->ghost.y) < PACMAN_SIZE)
            {
            printf("Game Over! The ghost caught Pac-Man.\n");
            curr_level_num = 5;
            // SDL_Quit();
            //exit(0);
            }
        }
    
    // PACMAN ARROWS COLLISION
    if (shield == 0) {
    if (fabs(game->pacman.x - game->arrow_up.x) < PACMAN_SIZE && fabs(game->pacman.y - game->arrow_up.y) < PACMAN_SIZE) {
        printf("Game Over! The arrow caught Pac-Man.\n");
        curr_level_num = 5;
        //SDL_Quit();
        //exit(0);
    }

    if (fabs(game->pacman.x - game->arrow_down.x) < PACMAN_SIZE && fabs(game->pacman.y - game->arrow_down.y) < PACMAN_SIZE) {
        printf("Game Over! The arrow caught Pac-Man.\n");
        curr_level_num = 5;
        //SDL_Quit();
        //exit(0);
    }
}  
    
    check_level_end(game->pacman.x, game->pacman.y, game);
    return quit;
}

void renderGame(SDL_Renderer *renderer, GameState *game)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    arrow_logic(game, curr_level_num);
    if (curr_level_num == 1)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (curr_level[y][x] == 1)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                }
                else if (curr_level[y][x] == 2)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
                }
            }
        }
    }
    else if (curr_level_num == 3)
    {

        float pacman_x = game->pacman.x;
        float pacman_y = game->pacman.y;
        int pacman_xTile = pacman_x / TILE_SIZE;
        int pacman_yTile = pacman_y / TILE_SIZE;

        SDL_SetRenderDrawColor(renderer, 204, 204, 204, 255);
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (curr_level[y][x] == 1)
                { // check for wall
                    // check if xth,yth tile is in a 4block radius of pacman tile;
                    if (abs(x - pacman_xTile) < 4 && abs(y - pacman_yTile) < 4)
                    {
                        SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                        SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                    }
                }
            }
        }
    }
    else if (curr_level_num == 2)
    {

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (curr_level[y][x] == 1)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                }
                else if (curr_level[y][x] == 2)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        
        if(game->arrow_up.x != 0 || game->arrow_down.x != 0){
        SDL_Rect arrowUpRect = {game->arrow_up.x, game->arrow_up.y, 20, 54};
        SDL_RenderCopy(renderer, game->arrow_up_texture, NULL, &arrowUpRect);
        
        SDL_Rect arrowDownRect = {game->arrow_down.x, game->arrow_down.y,20, 54};
        SDL_RenderCopy(renderer, game->arrow_down_texture, NULL, &arrowDownRect);
        }
    }    
    else if (curr_level_num == 4){
    
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int y = 0; y < MAP_HEIGHT; y++){
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    if (curr_level[y][x] == 1)
                    {
                        SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                        SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                    }
                    else if (curr_level[y][x] == 2)
                    {
                        SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                        SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
                    }
                    else if(curr_level[y][x] == 3){
                        SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                        SDL_RenderCopy(renderer, curr_second_text(game->lava_flow_texture,game), NULL, &wallRect);
                    }
                }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        
        float arrow_down_x = game->arrow_down.x;
        float arrow_up_x = game->arrow_up.x;

        if(arrow_down_x > 2 || arrow_up_x > 2){
            SDL_Rect arrowUpRect = {game->arrow_up.x, game->arrow_up.y, 20, 54};
            SDL_RenderCopy(renderer, game->arrow_up_texture, NULL, &arrowUpRect);
        
            SDL_Rect arrowDownRect = {game->arrow_down.x, game->arrow_down.y, 20, 54};
            SDL_RenderCopy(renderer, game->arrow_down_texture, NULL, &arrowDownRect);
        }
    }
    else if (curr_level_num == 5)
    {

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (curr_level[y][x] == 1)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderFillRect(renderer, &wallRect);
                }
                else if (curr_level[y][x] == 2)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
                }
                 
            }
        }

        SDL_Rect game_overRect = {296,114,480,180};
        SDL_RenderCopy(renderer,game->game_over_texture,NULL,&game_overRect);

        SDL_Rect play_againRect = {1*TILE_SIZE+10,8*TILE_SIZE,0.5*336.2,0.5*108};
        SDL_RenderCopy(renderer,game->play_again_texture,NULL,&play_againRect);
        
        SDL_Rect quitRect = {17*TILE_SIZE+10,8*TILE_SIZE,0.5*169.2,0.5*108};
        SDL_RenderCopy(renderer,game->quit_texture,NULL,&quitRect);
        


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }

    if(shield == 0){
        SDL_Rect pacmanRect = {(int)game->pacman.x, (int)game->pacman.y, PACMAN_SIZE, PACMAN_SIZE};
        SDL_RenderCopy(renderer, curr_pacman_texture, NULL, &pacmanRect);
    }
    else{
        SDL_Rect pacmanRect = {(int)game->pacman.x, (int)game->pacman.y, PACMAN_SIZE, PACMAN_SIZE};
        SDL_RenderCopy(renderer, curr_shield_texture, NULL, &pacmanRect);
    }

    if(curr_level_num!=1 && curr_level_num!=2){    
        SDL_Rect ghostRect = {(int)game->ghost.x, (int)game->ghost.y, PACMAN_SIZE, PACMAN_SIZE};
        SDL_RenderCopy(renderer, game->ghost_texture, NULL, &ghostRect);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Pac-Man Clone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, 596, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!window || !renderer)
    {
        printf("Failed to create window or renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *pacmanUpSurface = IMG_Load("images/pacman-up.png");
    if (!pacmanUpSurface)
    {
        printf("Could not load Pac-Man-Up image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *pacmanDownSurface = IMG_Load("images/pacman-down.png");
    if (!pacmanDownSurface)
    {
        printf("Could not load Pac-Man-Down image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *pacmanBackSurface = IMG_Load("images/pacman-back.png");
    if (!pacmanBackSurface)
    {
        printf("Could not load Pac-Man-Back image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *pacmanFrontSurface = IMG_Load("images/pacman-front.png");
    if (!pacmanFrontSurface)
    {
        printf("Could not load Pac-Man-Front image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *shieldFrontSurface = IMG_Load("images/pacman-front-shield.png");
    if (!pacmanFrontSurface)
    {
        printf("Could not load Pac-Shield-Front image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *shieldUpSurface = IMG_Load("images/pacman-up-shield.png");
    if (!pacmanFrontSurface)
    {
        printf("Could not load Pac-Shield-Up image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *shieldBackSurface = IMG_Load("images/pacman-back-shield.png");
    if (!pacmanFrontSurface)
    {
        printf("Could not load Pac-Shield-Back image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *shieldDownSurface = IMG_Load("images/pacman-down-shield.png");
    if (!pacmanFrontSurface)
    {
        printf("Could not load Pac-Shield-Down: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *ghostSurface = IMG_Load("images/ghost.png");
    if (!ghostSurface)
    {
        printf("Could not load Ghost image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *arrowUpSurface = IMG_Load("images/arrow-up.png");
    if (!arrowUpSurface)
    {
        printf("Could not load arrow-up image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *arrowDownSurface = IMG_Load("images/arrow-down.png");
    if (!arrowDownSurface)
    {
        printf("Could not load arrow-up2 image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *mossSurface = IMG_Load("images/moss.png");
    if (!mossSurface)
    {
        printf("Could not load moss image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *waterSurface = IMG_Load("images/water.png");
    if (!waterSurface)
    {
        printf("Could not load water image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *stoneSurface = IMG_Load("images/stone.jpg");
    if (!stoneSurface)
    {
        printf("Could not load stoneimage: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *lavaSurface = IMG_Load("images/lava.jpg");
    if (!lavaSurface)
    {
        printf("Could not load lava image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *lava_flowSurface = IMG_Load("images/lava_flow.jpg");
    if (!lava_flowSurface)
    {
        printf("Could not load lava image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *water_flowSurface = IMG_Load("images/water_flow.jpg");
    if (!water_flowSurface)
    {
        printf("Could not load water image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *Game_overSurface = IMG_Load("images/Game_over.jpg");
    if (!Game_overSurface)
    {
        printf("Could not load water image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Surface *Play_againSurface = IMG_Load("images/play_again.png");
    if (!Play_againSurface)
    {
        printf("Could not load play-again image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Surface *QuitSurface = IMG_Load("images/quit.png");
    if (!QuitSurface)
    {
        printf("Could not load quit image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    GameState game;
    game.arrow_up.y = -1; // Start off-screen
    game.arrow_up.x = -1;
    game.arrow_down.y = -1; // Start off-screen
    game.arrow_down.x = -1;
    
    game.pacman.vx = 0;
    game.pacman.vy = 0;

    game.pacman_up_texture = SDL_CreateTextureFromSurface(renderer, pacmanUpSurface);
    game.pacman_back_texture = SDL_CreateTextureFromSurface(renderer, pacmanBackSurface);
    game.pacman_down_texture = SDL_CreateTextureFromSurface(renderer, pacmanDownSurface);
    game.pacman_front_texture = SDL_CreateTextureFromSurface(renderer, pacmanFrontSurface);
    game.pacman_up_shield_texture = SDL_CreateTextureFromSurface(renderer, shieldUpSurface);
    game.pacman_back_shield_texture = SDL_CreateTextureFromSurface(renderer, shieldBackSurface);
    game.pacman_down_shield_texture = SDL_CreateTextureFromSurface(renderer, shieldDownSurface);
    game.pacman_front_shield_texture = SDL_CreateTextureFromSurface(renderer, shieldFrontSurface);
    game.ghost_texture = SDL_CreateTextureFromSurface(renderer, ghostSurface);
    game.arrow_up_texture = SDL_CreateTextureFromSurface(renderer, arrowUpSurface);
    game.arrow_down_texture = SDL_CreateTextureFromSurface(renderer, arrowDownSurface);
    game.moss_texture = SDL_CreateTextureFromSurface(renderer, mossSurface);
    game.stone_texture = SDL_CreateTextureFromSurface(renderer, stoneSurface);
    game.water_texture = SDL_CreateTextureFromSurface(renderer, waterSurface);
    game.lava_texture = SDL_CreateTextureFromSurface(renderer, lavaSurface);
    game.lava_flow_texture = SDL_CreateTextureFromSurface(renderer, lava_flowSurface);
    game.water_flow_texture = SDL_CreateTextureFromSurface(renderer, water_flowSurface);
    game.game_over_texture = SDL_CreateTextureFromSurface(renderer,Game_overSurface);
    game.play_again_texture = SDL_CreateTextureFromSurface(renderer,Play_againSurface);
    game.quit_texture = SDL_CreateTextureFromSurface(renderer,QuitSurface);



    SDL_FreeSurface(pacmanUpSurface);
    SDL_FreeSurface(pacmanDownSurface);
    SDL_FreeSurface(pacmanFrontSurface);
    SDL_FreeSurface(pacmanBackSurface);
    SDL_FreeSurface(shieldBackSurface);
    SDL_FreeSurface(shieldUpSurface);
    SDL_FreeSurface(shieldFrontSurface);
    SDL_FreeSurface(shieldDownSurface);
    SDL_FreeSurface(ghostSurface);
    SDL_FreeSurface(arrowUpSurface);
    SDL_FreeSurface(arrowDownSurface);
    SDL_FreeSurface(mossSurface);
    SDL_FreeSurface(stoneSurface);
    SDL_FreeSurface(waterSurface);
    SDL_FreeSurface(lavaSurface);
    SDL_FreeSurface(lava_flowSurface);
    SDL_FreeSurface(water_flowSurface);
    SDL_FreeSurface(Game_overSurface);
    SDL_FreeSurface(Play_againSurface);
    SDL_FreeSurface(QuitSurface);




    Uint32 lastTime = SDL_GetTicks();
    
    int quit = 0;

    //intial Spawn
    set_pacman_spawn(&game.pacman.x, &game.pacman.y, curr_level_num);
    curr_pacman_texture = game.pacman_front_texture;
    curr_shield_texture = game.pacman_front_shield_texture;
    set_ghost_spawn(&game.ghost.x, &game.ghost.y, curr_level_num,&game);

    while (!quit)
    {
        currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        moveGhost(&game, deltaTime);

        // Change level only when needed
        if (curr_level_num != prev_level_num)
        {
            if (curr_level_num == 1)
            {
                array_copy(curr_level, level1);
            }
            else if (curr_level_num == 2)
            {
                array_copy(curr_level, level2);
            }
            else if (curr_level_num == 3)
            {
                array_copy(curr_level, level3);
            }
            else if (curr_level_num == 4)
            {
                array_copy(curr_level, level4);
            }
            else if (curr_level_num == 5)
            {
                array_copy(curr_level, level5);
            }

            // Respawn Pac-Man when switching levels
            set_pacman_spawn(&game.pacman.x, &game.pacman.y, curr_level_num);
            //set_ghost_spawn(&game.ghost.x, &game.ghost.y, curr_level_num,game);
            game.pacman.vx = 0;
            game.pacman.vy = 0;

            prev_level_num = curr_level_num; // Update previous level tracker
        }

        quit = check_events(window, &game, SPEED, deltaTime);
        renderGame(renderer, &game);
    }
    SDL_DestroyTexture(game.pacman_up_texture);
    SDL_DestroyTexture(game.pacman_down_texture);
    SDL_DestroyTexture(game.pacman_front_texture);
    SDL_DestroyTexture(game.pacman_back_texture);
    SDL_DestroyTexture(game.arrow_up_texture);
    SDL_DestroyTexture(game.arrow_down_texture);
    SDL_DestroyTexture(game.moss_texture);
    SDL_DestroyTexture(game.stone_texture);
    SDL_DestroyTexture(game.water_texture);
    SDL_DestroyTexture(game.lava_texture);
    SDL_DestroyTexture(game.lava_flow_texture);
    SDL_DestroyTexture(game.water_flow_texture);
    SDL_DestroyTexture(game.game_over_texture);
    SDL_DestroyTexture(game.play_again_texture);
    SDL_DestroyTexture(game.quit_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}