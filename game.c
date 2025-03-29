#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define PACMAN_SIZE 100

// Structure to represent an entity (Pac-Man)
typedef struct {
    float x, y;  // Position
    float vx, vy; // Velocity
} Entity;

// Structure to represent game state
typedef struct {
    Entity pacman;
    SDL_Texture *pacman_texture;
} GameState;

// Function to handle user input and update entity movement
int check_events(SDL_Window *window, GameState *game, float speed, float deltaTime) {
    int quit = 0;
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                }
                break;
        }
    }

    // Get keyboard state
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    float acceleration = speed * deltaTime;

    // Handle horizontal movement
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        game->pacman.vx = -acceleration;
    } else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        game->pacman.vx = acceleration;
    } else {
        game->pacman.vx *= 0.85f; // Apply friction
    }

    // Handle vertical movement
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        game->pacman.vy = -acceleration;
    } else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        game->pacman.vy = acceleration;
    } else {
        game->pacman.vy *= 0.85f; // Apply friction
    }

    // Update position based on velocity
    game->pacman.x += game->pacman.vx;
    game->pacman.y += game->pacman.vy;


    // Prevent Pac-Man from moving out of bounds
    if (game->pacman.x < 0)
    game->pacman.x = 0;
    if (game->pacman.x > SCREEN_WIDTH - PACMAN_SIZE)
    game->pacman.x = SCREEN_WIDTH - PACMAN_SIZE;

    if (game->pacman.y < 0)
    game->pacman.y = 0;
    if (game->pacman.y > SCREEN_HEIGHT - PACMAN_SIZE)
    game->pacman.y = SCREEN_HEIGHT - PACMAN_SIZE;


    return quit;
}

// Function to render game objects
void renderGame(SDL_Renderer *renderer, GameState *game) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Rect pacmanRect = { (int)game->pacman.x, (int)game->pacman.y, 100, 100 };
    SDL_RenderCopy(renderer, game->pacman_texture, NULL, &pacmanRect);
    
    SDL_RenderPresent(renderer);
}

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create window and renderer
    SDL_Window *window = SDL_CreateWindow("Smooth Entity Movement", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!window || !renderer) {
        printf("Failed to create window or renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Load Pac-Man texture
    SDL_Surface *pacmanSurface = IMG_Load("pacman-png-25200.png");
    if (!pacmanSurface) {
        printf("Could not load Pac-Man image\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Initialize game state
    GameState game = { { 320.0f, 240.0f, 0.0f, 0.0f }, SDL_CreateTextureFromSurface(renderer, pacmanSurface) };
    SDL_FreeSurface(pacmanSurface);

    float speed = 400.0f;
    Uint32 lastTime = SDL_GetTicks();
    int quit = 0;
    
    // Game loop
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        quit = check_events
    (window, &game, speed, deltaTime);
        renderGame(renderer, &game);
    }

    // Cleanup
    SDL_DestroyTexture(game.pacman_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
