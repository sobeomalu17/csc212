#include "test.h"
#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int IMAGE_WIDTH = SCREEN_WIDTH + 300;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
int speed = 0.1;
int CameraX = 0;
int textureWidthDiff = IMAGE_WIDTH - SCREEN_WIDTH;

class Ball {
public:
    float x, y; // Changed from int to float
    int velocityX, velocityY; // Change velocity to handle direction
    SDL_Rect spriteClip; // Clip of the current sprite
    int spriteWidth, spriteHeight; // Dimensions of each sprite
    int currentSprite; // Index of the current sprite

    Ball(float posX, float posY, int velX, int velY, int spriteW, int spriteH) : x(posX), y(posY), velocityX(velX), velocityY(velY), spriteWidth(spriteW), spriteHeight(spriteH), currentSprite(0) {
        // Initialize sprite clip
        spriteClip.x = 0;
        spriteClip.y = 0;
        spriteClip.w = spriteWidth;
        spriteClip.h = spriteHeight;
    }

    void animate() {
        // Update sprite clip for animation
        spriteClip.x = currentSprite * spriteWidth;
    }

    void update() {
        // Update ball position
        x += velocityX;
        y += velocityY;

        // Check if the ball hits the side walls
        if (x > SCREEN_WIDTH - spriteWidth) {
            x = SCREEN_WIDTH - spriteWidth; // Set ball position to the right edge of the screen
            velocityX *= -1; // Reverse direction
        }
        else if (x < 0) {
            x = 0; // Set ball position to the left edge of the screen
            velocityX *= -1; // Reverse direction
        }

        // Check if the ball hits the top or bottom walls
        if (y > SCREEN_HEIGHT - spriteHeight) {
            y = SCREEN_HEIGHT - spriteHeight; // Set ball position to the bottom edge of the screen
            velocityY *= -1; // Reverse direction
        }
        else if (y < 0) {
            y = 0; // Set ball position to the top edge of the screen
            velocityY *= -1; // Reverse direction
        }

        animate();
    }
};

SDL_Texture* loadTexture(const std::string& path) {
    // The final texture
    SDL_Texture* newTexture = NULL;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else {
        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

bool initSDL()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("2D Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Set renderer color
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Allows for (Initializes) Image Loading abilities using SDL_image library
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

void closeSDL()
{
    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        printf("Failed to initialize SDL!\n");
        return -1;
    }

    SDL_Texture* ballTexture = loadTexture("./Assets/ball.png"); // Load ball texture

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Seed random number generator
    srand(time(NULL));

    std::vector<Ball> balls;
    // Load ball spritesheet dimensions
    int ballSpriteWidth = 512;
    int ballSpriteHeight = 512;
    balls.push_back(Ball(800, SCREEN_HEIGHT - 155, 4, 4, ballSpriteWidth, ballSpriteHeight)); // Create a ball that starts at the left edge of the screen

    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        SDL_RenderClear(gRenderer);

        // Render and update balls
        for (Ball& ball : balls) {
            ball.update();
            SDL_Rect ballDest = { static_cast<int>(ball.x), static_cast<int>(ball.y), ball.spriteWidth, ball.spriteHeight };
            SDL_RenderCopy(gRenderer, ballTexture, &ball.spriteClip, &ballDest);
        }

        // Update screen
        SDL_RenderPresent(gRenderer);

        // Add a small delay to control the frame rate
        SDL_Delay(10); // Adjust as needed for desired frame rate
    }

    // Destroy textures
    SDL_DestroyTexture(ballTexture);

    // Close SDL
    closeSDL();

    return 0;
}