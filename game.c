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

// GameState holds the entire state of the game including entities and textures.
// This structure makes it easier to manage and pass around all relevant game data.

typedef struct
{
    Entity pacman;  // The main player character entity (Pac-Man)
    Entity ghost;   // The enemy character entity (Ghost)
    Entity arrow_up;   // Possibly a UI element or directional projectile entity (arrow pointing up)
    Entity arrow_down; // Possibly a UI element or directional projectile entity (arrow pointing down)

    // Textures for Pac-Man facing different directions
    SDL_Texture *pacman_up_texture;     // Texture for Pac-Man facing upward
    SDL_Texture *pacman_front_texture;  // Texture for Pac-Man facing forward (usually right)
    SDL_Texture *pacman_down_texture;   // Texture for Pac-Man facing downward
    SDL_Texture *pacman_back_texture;   // Texture for Pac-Man facing backward (left)

    // Shielded textures for Pac-Man when a shield power-up is active
    SDL_Texture *pacman_up_shield_texture;     // Shielded texture for Pac-Man facing upward
    SDL_Texture *pacman_front_shield_texture;  // Shielded texture for Pac-Man facing forward
    SDL_Texture *pacman_down_shield_texture;   // Shielded texture for Pac-Man facing downward
    SDL_Texture *pacman_back_shield_texture;   // Shielded texture for Pac-Man facing backward

    SDL_Texture *ghost_texture;        // Texture for the ghost enemy
    SDL_Texture *arrow_up_texture;     // Texture for arrow pointing up
    SDL_Texture *arrow_down_texture;   // Texture for arrow pointing down

    // Terrain textures
    SDL_Texture *moss_texture;         // Texture for moss tiles (probably walkable ground)
    SDL_Texture *stone_texture;        // Texture for stone tiles
    SDL_Texture *water_texture;        // Texture for static water tiles
    SDL_Texture *lava_texture;         // Texture for static lava tiles
    SDL_Texture *lava_flow_texture;    // Animated or active lava (e.g., flowing lava)
    SDL_Texture *water_flow_texture;   // Animated or active water (e.g., flowing water)

    // UI or game state screen textures
    SDL_Texture *game_over_texture;    // Texture for game over screen
    SDL_Texture *play_again_texture;   // Texture for play again button
    SDL_Texture *quit_texture;         // Texture for quit button

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
int curr_level_num = 1;
int prev_level_num = 0;
int shield = 0;
SDL_Texture *curr_pacman_texture;
SDL_Texture *curr_shield_texture;
Uint32 lastSpacePressTime = 0;
Uint32 currentTime = 0;
Uint32 spacePressStartTime = 0;
Uint32 cooldown_start=0;

// Copies the contents of one 2D array (level_array) into another (curr_level).
// Useful for initializing or resetting the current level layout in the game.

void array_copy(int curr_level[13][20], int level_array[13][20])
{
    // Loop over all rows (13 rows in total)
    for (int i = 0; i < 13; i++)
    {
        // Loop over all columns in each row (20 columns in total)
        for (int j = 0; j < 20; j++)
        {
            // Copy the value from level_array to curr_level at the same position
            curr_level[i][j] = level_array[i][j];
        }
    }
}


// Sets the initial spawn position (x, y) of Pac-Man based on the current level.
// The position is calculated in pixels based on tile coordinates.
// Uses TILE_SIZE for the grid system, and centers Pac-Man within the tile using PACMAN_SIZE.

void set_pacman_spawn(float *x, float *y, int curr_level)
{
    switch (curr_level)
    {
    case 1:
        // Level 1 spawn: tile (1,1)
        *x = TILE_SIZE * 1 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 1 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 2:
        // Level 2 spawn: tile (0,6)
        *x = TILE_SIZE * 0 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 6 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 3:
        // Level 3 spawn: tile (1,0)
        *x = TILE_SIZE * 1 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 0 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 4:
        // Level 4 spawn: tile (0,9)
        *x = TILE_SIZE * 0 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 5:
        // Level 5 spawn: tile (9,6)
        *x = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 6 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    default:
        // If the level doesn't match any case, do nothing (Pac-Man won't be positioned)
        break;
    }
}


// Sets the spawn position (x, y) of the ghost based on the current level.
// Takes in the current level number and the game state to optionally base spawn logic on Pac-Man's position.

void set_ghost_spawn(float *x, float *y, int curr_level, GameState *game)
{
    switch (curr_level)
    {
    case 1:
        // Level 1: Ghost starts at the top-left corner of the screen
        *x = 0;
        *y = 0;
        break;

    case 2:
        // Level 2: Ghost also starts at the top-left corner
        *x = 0;
        *y = 0;
        break;

    case 3:
        // Level 3: Ghost spawns near bottom-right, tile (10,9), centered in the tile
        *x = TILE_SIZE * 10 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 9 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 4:
        // Level 4: Ghost spawns at tile (8,5), centered in the tile
        *x = TILE_SIZE * 8 + (TILE_SIZE - PACMAN_SIZE) / 2;
        *y = TILE_SIZE * 5 + (TILE_SIZE - PACMAN_SIZE) / 2;
        break;

    case 5:
        // Level 5: Ghost spawns close to Pac-Man's current x position, one tile to the right
        *x = game->pacman.x + (0.95 * TILE_SIZE);  // Slightly less than one tile to prevent overlapping
        *y = game->pacman.y;  // Same vertical position as Pac-Man
        break;
    }
}



// Checks whether the given (x, y) position collides with any solid object in the level (like walls or obstacles).
// Returns 1 if there is a collision; 0 if the position is clear.

int checkCollision(float x, float y)
{
    // Convert position (x, y) in pixels to tile coordinates for each corner of the entity.
    // These represent the corners of the bounding box around the entity (e.g., Pac-Man).

    int tileX1 = (int)(x / TILE_SIZE);                     // left side of entity
    int tileY1 = (int)(y / TILE_SIZE);                     // top side of entity
    int tileX2 = (int)((x + PACMAN_SIZE - 1) / TILE_SIZE); // right side of entity
    int tileY2 = (int)((y + PACMAN_SIZE - 1) / TILE_SIZE); // bottom side of entity

    // Optional exit condition (currently commented out):
    // Example: if Pac-Man reaches tile (19, 6), the game exits.
    // This could be used as a level exit point or teleportation trigger.
    // if(tileY1 == 6 && tileX1 == 19){
    //     SDL_Quit();
    // }

    // Check if the calculated tile coordinates are outside the level bounds
    if (tileX1 < 0 || tileX2 >= MAP_WIDTH || tileY1 < 0 || tileY2 >= MAP_HEIGHT)
        return 1; // Out of bounds = collision

    // Check if any corner of the entity is inside a wall or obstacle.
    // Level tiles are checked for collision based on certain tile values:
    // 1 = stone wall, 2 = water, 3 = lava — all considered impassable

    // Check for stone (tile value 1)
    if (curr_level[tileY1][tileX1] == 1 || curr_level[tileY1][tileX2] == 1 ||
        curr_level[tileY2][tileX1] == 1 || curr_level[tileY2][tileX2] == 1)
    {
        return 1;
    }

    // Check for water (tile value 2)
    if (curr_level[tileY1][tileX1] == 2 || curr_level[tileY1][tileX2] == 2 ||
        curr_level[tileY2][tileX1] == 2 || curr_level[tileY2][tileX2] == 2)
    {
        return 1;
    }

    // Check for lava (tile value 3)
    if (curr_level[tileY1][tileX1] == 3 || curr_level[tileY1][tileX2] == 3 ||
        curr_level[tileY2][tileX1] == 3 || curr_level[tileY2][tileX2] == 3)
    {
        return 1;
    }

    // No collision with solid objects — movement is allowed
    return 0;
}


// Moves the ghost towards Pac-Man in a straight line (basic AI movement).
// Uses deltaTime to ensure smooth movement regardless of frame rate.
// Movement can be disabled for certain levels by setting speed to 0.

void moveGhost(GameState *game, float deltaTime)
{
    float speed = 100.0f; // Default ghost speed in pixels per second (can be tuned)

    // On certain levels (1, 2, or 5), ghost movement is disabled
    if (curr_level_num == 5 || curr_level_num == 1 || curr_level_num == 2)
    {
        speed = 0;
    }

    // Calculate the vector from ghost to Pac-Man
    float dx = game->pacman.x - game->ghost.x;
    float dy = game->pacman.y - game->ghost.y;

    // Compute the distance between the ghost and Pac-Man
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0)
    {
        // Normalize the direction vector and multiply by speed and deltaTime
        // This ensures constant speed and frame-rate-independent movement
        game->ghost.vx = (dx / distance) * speed * deltaTime;
        game->ghost.vy = (dy / distance) * speed * deltaTime;
    }

    // Predict the new position
    float newX = game->ghost.x + game->ghost.vx;
    float newY = game->ghost.y + game->ghost.vy;

    // Attempt to move in the X direction if no collision
    if (!checkCollision(newX, game->ghost.y))
    {
        game->ghost.x = newX;
    }

    // Attempt to move in the Y direction if no collision
    if (!checkCollision(game->ghost.x, newY))
    {
        game->ghost.y = newY;
    }
}

// Returns the primary terrain texture to be used for rendering based on the current level.
// This is used to select a background tile type (like moss, water, stone, etc.) depending on the level number.

SDL_Texture *curr_primary_text(int curr_level, GameState *game)
{
    switch (curr_level)
    {
    case 1:
        // Level 1 uses moss as the base texture
        return game->moss_texture;

    case 2:
        // Level 2 uses water as the base texture
        return game->water_texture;

    case 3:
        // Level 3 uses stone as the base texture
        return game->stone_texture;

    case 4:
        // Level 4 uses lava as the base texture
        return game->lava_texture;

    case 5:
        // Level 5 also uses lava as the base texture
        return game->lava_texture;
    }

    // NOTE: No default case. If an undefined level is passed, behavior is undefined (likely returns nothing).
    // You may consider adding a `default` return to prevent unexpected behavior.
}


// Returns the secondary terrain texture for the current level.
// This is used for dynamic or decorative tiles (e.g., flowing water or lava) that enhance the visuals or gameplay variety.

SDL_Texture *curr_second_text(int curr_level, GameState *game)
{
    switch (curr_level)
    {
    case 1:
        // Level 1 uses moss again as the secondary texture
        return game->moss_texture;

    case 2:
        // Level 2 uses animated flowing water as the secondary texture
        return game->water_flow_texture;

    case 3:
        // Level 3 reuses stone texture as both primary and secondary
        return game->stone_texture;

    case 4:
        // Level 4 uses flowing lava to complement the static lava base
        return game->lava_flow_texture;

    case 5:
        // Level 5 uses stone as a secondary element over lava
        return game->stone_texture;
    }

    // NOTE: No default case, so passing an undefined level number may return undefined behavior or garbage
}


// Controls the behavior of arrows that move vertically across the screen (up and down).
// These arrows act as dynamic hazards in levels 2 and 4, but are disabled in levels 1, 3, and 5.

void arrow_logic(GameState *game, int level)
{
    // Disable arrows completely for levels 1, 3, and 5
    if (level == 1 || level == 3 || level == 5)
    {
        game->arrow_up.x = 0;
        game->arrow_down.x = 0;
    }
    else
    {
        int arrow_speed = 5; // Speed at which the arrows move vertically

        // If the upward arrow is inactive (-1), randomly respawn it at bottom
        if (game->arrow_up.x == -1)
        {   
            // Random x position within screen bounds, avoiding leftmost 3 tiles
            int x1 = (rand() % (SCREEN_WIDTH - 3 * TILE_SIZE)) + (3 * TILE_SIZE);
            game->arrow_up.x = x1 + (TILE_SIZE / 2);  // Centered within tile
            game->arrow_up.y = 11 * TILE_SIZE;        // Start from bottom of map
        }

        // If the downward arrow is inactive (-1), randomly respawn it at top
        if (game->arrow_down.x == -1)
        { 
            int x2 = (rand() % (SCREEN_WIDTH - 3 * TILE_SIZE)) + (3 * TILE_SIZE);
            game->arrow_down.x = x2 + (TILE_SIZE / 2);  // Centered
            game->arrow_down.y = 0;                     // Start from top of map
        }

        // Move arrows: up arrow goes up, down arrow goes down
        game->arrow_up.y -= arrow_speed;
        game->arrow_down.y += arrow_speed;

        // Deactivate arrows once they go off screen
        if (game->arrow_up.y <= 0)
        {
            game->arrow_up.y = -1;
            game->arrow_up.x = -1;  // Set to -1 to mark as inactive
        }
        if (game->arrow_down.y >= 11 * TILE_SIZE)
        {
            game->arrow_down.y = -1;
            game->arrow_down.x = -1;  // Mark as inactive
        }
    }
}


// Function: check_level_end
// Purpose: Checks whether the player (Pac-Man) has reached the level's exit tile or triggered special tile-based events.
// If a level ends, it progresses to the next level, resets arrows, repositions the ghost, and modifies the level map if needed.
// Returns: 1 if the level ends, 0 otherwise.

int check_level_end(float x, float y, GameState *game)
{
    // Convert pixel coordinates to tile coordinates
    int tileX1 = (int)(x / TILE_SIZE); // Horizontal tile index
    int tileY1 = (int)(y / TILE_SIZE); // Vertical tile index

    switch (curr_level_num)
    {
    // --- LEVEL 1 EXIT ---
    case 1:
        if (tileY1 == 6 && tileX1 == 19) // If Pac-Man reaches exit tile
        {   
            printf("Ending level 1");
            curr_level_num = 2; // Move to level 2
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num, game); // Set ghost spawn for level 2
            game->arrow_up.x = -1; // Disable arrows for level 2
            game->arrow_down.x = -1;
            return 1; // Signal level end
        }
        break;

    // --- LEVEL 2 EXIT ---
    case 2:
        if (tileY1 == 11 && tileX1 == 18)
        {
            printf("Ending level 2");
            curr_level_num = 3; // Move to level 3
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num, game);
            game->arrow_up.x = 0; // Enable arrows for level 3
            game->arrow_down.x = 0;
            return 1;
        }
        break;

    // --- LEVEL 3 EXIT ---
    case 3:
        if (tileY1 == 8 && tileX1 == 19)
        {
            printf("Ending level 3");
            curr_level_num = 4; // Move to level 4
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num, game);
            game->arrow_up.x = -1; // Disable arrows
            game->arrow_down.x = -1;
            return 1;
        }
        break;

    // --- LEVEL 4 SPECIAL TILE EVENTS + EXIT ---
    case 4:
        // Tile interaction logic: open a path
        if (tileY1 == 1 && tileX1 == 1){
            level4[1][2] = 0;
            level4[1][3] = 0;
            level4[1][4] = 0;
        }

        // Re-close the path
        if(tileY1 == 1 && tileX1 == 5){
            level4[1][2] = 1;
            level4[1][3] = 1;
            level4[1][4] = 1;
        }

        // More complex tile logic (open alternative paths, lock others)
        if(tileY1 == 1 && tileX1 == 9){
            level4[1][10] = 0;
            level4[2][9] = 0;
            level4[3][9] = 0;
            level4[4][9] = 0;

            level4[1][13] = 1;
            level4[3][8] = 1;
            level4[3][10] = 1;
        }

        // Revert those changes
        if(tileY1 == 5 && tileX1 == 9){
            level4[1][10] = 0;
            level4[2][9] = 1;
            level4[3][9] = 2;
            level4[4][9] = 1;

            level4[1][13] = 0;
            level4[3][8] = 2;
            level4[3][10] = 2;
        }

        // Final tile to end level 4
        if(tileY1 == 11 && tileX1 == 18){
            printf("Ending level ");
            curr_level_num = 5; // Move to level 5
            set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num, game);
        }

        // Apply modified level 4 layout to the current map
        array_copy(curr_level, level4);
        break;

    // --- LEVEL 5: MULTIPLE ENDING OR RESET TRIGGERS ---
    case 5:
        // Always reset ghost spawn for level 5
        set_ghost_spawn(&game->ghost.x, &game->ghost.y, curr_level_num, game);

        // Loop back to level 1 from specific tiles
        if(tileY1 == 8 && (tileX1 == 1 || tileX1 == 2 || tileX1 == 3)){
            curr_level_num = 1;
        }

        // Exit game on reaching specific tiles
        else if(tileY1 == 8 && tileX1 == 17){
            curr_level_num = 1;
            SDL_Quit(); // Game over
        }
        else if(tileY1 == 8 && tileX1 == 16){
            SDL_Quit(); // Alternate ending
        }
        break;

    // --- DEFAULT: Do nothing ---
    default:
        break;
    }

    // No level-end condition met
    return 0;
}



// Function: check_events
// Purpose: Handles all input and gameplay events per frame, including movement, shield logic, collision detection, and level progression.
// Returns: 1 if the game should quit, 0 otherwise.

int check_events(SDL_Window *window, GameState *game, float speed, float deltaTime)
{   
    int quit = 0;
    SDL_Event event;

    // Process all pending SDL events
    while (SDL_PollEvent(&event))
    {
        // Exit game if window is closed or Escape key is pressed
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
        {
            quit = 1;
        }
    }

    // Get current keyboard state
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    float acceleration = speed * deltaTime;
    float newX = game->pacman.x;
    float newY = game->pacman.y;

    // --- Pac-Man Movement Input ---
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
        game->pacman.vx *= DECAY_RATE; // Apply velocity decay when idle

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

    // --- Shield Mechanic with Spacebar ---
    if (keys[SDL_SCANCODE_SPACE]) {
        if (currentTime - cooldown_start > 3000) {  // 3-second cooldown after previous shield
            if (spacePressStartTime == 0) {
                spacePressStartTime = currentTime;  // Start hold duration
                cooldown_start = currentTime;       // Start cooldown
            }
            lastSpacePressTime = currentTime;

            // Disable shield if held for more than 1 second
            if (currentTime - spacePressStartTime >= 1000) {
                shield = 0;
            } else {
                shield = 1;
            }
        }
    } 
    else {
        spacePressStartTime = 0;  // Reset on release
    }

    // Disable shield 1 second after a tap
    if (currentTime - lastSpacePressTime >= 1000) {
        shield = 0;
    }

    // --- Collision Detection With Walls ---
    if (!checkCollision(newX, game->pacman.y))
        game->pacman.x = newX;
    if (!checkCollision(game->pacman.x, newY))
        game->pacman.y = newY;

    // --- Ghost Collision (except on level 5) ---
    if (curr_level_num != 5) {
        if (fabs(game->pacman.x - game->ghost.x) < PACMAN_SIZE &&
            fabs(game->pacman.y - game->ghost.y) < PACMAN_SIZE)
        {
            printf("Game Over! The ghost caught Pac-Man.\n");
            curr_level_num = 5;  // Send player to level 5 as a game over screen
        }
    }

    // --- Arrow Collision (if shield is off) ---
    if (shield == 0) {
        if (fabs(game->pacman.x - game->arrow_up.x) < PACMAN_SIZE &&
            fabs(game->pacman.y - game->arrow_up.y) < PACMAN_SIZE)
        {
            printf("Game Over! The arrow caught Pac-Man.\n");
            curr_level_num = 5;
        }

        if (fabs(game->pacman.x - game->arrow_down.x) < PACMAN_SIZE &&
            fabs(game->pacman.y - game->arrow_down.y) < PACMAN_SIZE)
        {
            printf("Game Over! The arrow caught Pac-Man.\n");
            curr_level_num = 5;
        }
    }

    // --- Check if Pac-Man reached level-end tile ---
    check_level_end(game->pacman.x, game->pacman.y, game);

    return quit;
}


void renderGame(SDL_Renderer *renderer, GameState *game)
{
    // Clear the screen to black before rendering
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Update arrow position based on level logic
    arrow_logic(game, curr_level_num);

    // --- Level 1 Rendering: Standard tiles with primary & secondary texture ---
    if (curr_level_num == 1)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                if (curr_level[y][x] == 1)  // Primary wall
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                }
                else if (curr_level[y][x] == 2)  // Secondary wall or item
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
                }
            }
        }
    }

    // --- Level 3 Rendering: "Dark vision" style based on Pac-Man's proximity ---
    else if (curr_level_num == 3)
    {
        float pacman_x = game->pacman.x;
        float pacman_y = game->pacman.y;
        int pacman_xTile = pacman_x / TILE_SIZE;
        int pacman_yTile = pacman_y / TILE_SIZE;

        SDL_SetRenderDrawColor(renderer, 204, 204, 204, 255); // Light gray for visibility
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                // Render only walls within 4-tile radius of Pac-Man
                if (curr_level[y][x] == 1 && abs(x - pacman_xTile) < 4 && abs(y - pacman_yTile) < 4)
                {
                    SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                }
            }
        }
    }

    // --- Level 2 Rendering: Similar to level 1, with arrows added ---
    else if (curr_level_num == 2)
    {
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

        // Render arrow projectiles if initialized (non-zero x)
        if (game->arrow_up.x != 0 || game->arrow_down.x != 0) {
            SDL_Rect arrowUpRect = {game->arrow_up.x, game->arrow_up.y, 20, 54};
            SDL_RenderCopy(renderer, game->arrow_up_texture, NULL, &arrowUpRect);

            SDL_Rect arrowDownRect = {game->arrow_down.x, game->arrow_down.y, 20, 54};
            SDL_RenderCopy(renderer, game->arrow_down_texture, NULL, &arrowDownRect);
        }
    }

    // --- Level 4 Rendering: Add lava tiles and arrows ---
    else if (curr_level_num == 4)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                if (curr_level[y][x] == 1)
                    SDL_RenderCopy(renderer, curr_primary_text(curr_level_num, game), NULL, &wallRect);
                else if (curr_level[y][x] == 2)
                    SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
                else if (curr_level[y][x] == 3) // Lava tile
                    SDL_RenderCopy(renderer, game->lava_flow_texture, NULL, &wallRect);
            }
        }

        // Arrows render only if they're visibly active (position check)
        if (game->arrow_down.x > 2 || game->arrow_up.x > 2) {
            SDL_Rect arrowUpRect = {game->arrow_up.x, game->arrow_up.y, 20, 54};
            SDL_RenderCopy(renderer, game->arrow_up_texture, NULL, &arrowUpRect);

            SDL_Rect arrowDownRect = {game->arrow_down.x, game->arrow_down.y, 20, 54};
            SDL_RenderCopy(renderer, game->arrow_down_texture, NULL, &arrowDownRect);
        }
    }

    // --- Level 5: Game Over Screen ---
    else if (curr_level_num == 5)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                SDL_Rect wallRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                if (curr_level[y][x] == 1)
                    SDL_RenderFillRect(renderer, &wallRect); // Just black wall
                else if (curr_level[y][x] == 2)
                    SDL_RenderCopy(renderer, curr_second_text(curr_level_num, game), NULL, &wallRect);
            }
        }

        // Show game over graphic
        SDL_Rect game_overRect = {296, 114, 480, 180};
        SDL_RenderCopy(renderer, game->game_over_texture, NULL, &game_overRect);

        // Show "Play Again" and "Quit" buttons
        SDL_Rect play_againRect = {1 * TILE_SIZE + 10, 8 * TILE_SIZE, 0.5 * 336.2, 0.5 * 108};
        SDL_RenderCopy(renderer, game->play_again_texture, NULL, &play_againRect);

        SDL_Rect quitRect = {17 * TILE_SIZE + 10, 8 * TILE_SIZE, 0.5 * 169.2, 0.5 * 108};
        SDL_RenderCopy(renderer, game->quit_texture, NULL, &quitRect);
    }

    // --- Render Pac-Man ---
    SDL_Rect pacmanRect = {(int)game->pacman.x, (int)game->pacman.y, PACMAN_SIZE, PACMAN_SIZE};
    if (shield == 0)
        SDL_RenderCopy(renderer, curr_pacman_texture, NULL, &pacmanRect);
    else
        SDL_RenderCopy(renderer, curr_shield_texture, NULL, &pacmanRect);

    // --- Render ghost in levels except 1 and 2 ---
    if (curr_level_num != 1 && curr_level_num != 2) {
        SDL_Rect ghostRect = {(int)game->ghost.x, (int)game->ghost.y, PACMAN_SIZE, PACMAN_SIZE};
        SDL_RenderCopy(renderer, game->ghost_texture, NULL, &ghostRect);
    }

    // Present all drawing to the screen
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
    
    //Set pacman initial pacman velocity
    game.pacman.vx = 0;
    game.pacman.vy = 0;
    //Load all teh textures
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


    // Free all surfaces after creating textures
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

        // Render the current state of the game
        renderGame(renderer, &game);
    }
    // --------------------- GAME LOOP END ----------------------

    // Clean up all textures
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

    // Destroy renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Clean up SDL
    SDL_Quit();

    return 0;
}