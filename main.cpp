#include "SDL.h"

// Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();

// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

void close()
{
    // Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char* args[])
{
    // Start up SDL and create window
    if(!init())
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        // Draw a rectangle
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = 100;
        rect.h = 100;

        // Fill the surface white
        SDL_FillRect(gScreenSurface, &rect, SDL_MapRGB(gScreenSurface->format, 0xFF, 0xFF, 0xFF));

        // Hack to get window to stay up
        SDL_Event e;
        bool quit = false;

        while(!quit)
        {
            while(SDL_PollEvent(&e ))
            {
                // Get keyboard events here
                quit = e.type == SDL_QUIT;
            }
        }
    }

    // Free resources and close SDL
    close();

    return 0;
}
