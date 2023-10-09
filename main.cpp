#include "SDL.h"
#include "include/Body.h"

#include "libs/Math922/include/Random.h"
#include "libs/Math922/include/Vec2.h"

bool init();
void close();
void Update();
void Render();

// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

SDL_Renderer* gRenderer = nullptr;

constexpr int SCREEN_WIDTH = 1550;
constexpr int SCREEN_HEIGHT = 900;

int PlanetRadius = 5;
Body* Planets = nullptr;
std::size_t PlanetsCount = 0;

Vec2F Center = Vec2F(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
float AttractionPower = 900.f;
float BaseVelocity = 400.f;
float MaxVelocity = 700.f;

int Time = 0;

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        // Create window
        gWindow = SDL_CreateWindow("Planets", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		// Enable VSync
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

		if (gWindow == nullptr)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
	    else
	    {
	        gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

			// Create planets
			int planetsToCreate = 1000;
			PlanetsCount = planetsToCreate;

			Planets = new Body[planetsToCreate];

			for (std::size_t i = 0; i < planetsToCreate; i++)
			{
				Planets[i].Position = Vec2F(Random::Range<float>(0.f, 200.f), Random::Range<float>(0.f, 200.f));
				Planets[i].Acceleration = Vec2F(0, 0);

				if (Random::Range<int>(0, 1) == 0)
				{
					Planets[i].Velocity = Vec2F(0, Random::Range<float>(1.f, 2.f));
					Planets[i].Mass = 1;
				}
				else
				{
					Planets[i].Velocity = Vec2F(Random::Range<float>(1.f, 2.f), 0);
					Planets[i].Mass = 2;
				}

				Planets[i].Velocity *= BaseVelocity;
			}

			Time = SDL_GetTicks64();
	    }
    }

    return success;
}

void close()
{
    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

void DrawCircle(int x, int y)
{
	for (int w = 0; w < PlanetRadius * 2; w++)
	{
		for (int h = 0; h < PlanetRadius * 2; h++)
		{
			int dx = PlanetRadius - w; // horizontal offset
			int dy = PlanetRadius - h; // vertical offset
			if ((dx*dx + dy*dy) <= (PlanetRadius * PlanetRadius))
			{
				SDL_RenderDrawPoint(gRenderer, x + dx, y + dy);
			}
		}
	}
}

void Update()
{
	const float deltaTime = (SDL_GetTicks64() - Time) / 1000.f;

	Time = SDL_GetTicks64();

	for (std::size_t i = 0; i < PlanetsCount; i++)
	{
		Planets[i].Update(deltaTime);

		// Calculate the vector from the center of the screen to the planet.
		Vec2F centerToPlanet = Center - Planets[i].Position;

		bool customOrbit = false;

		if (customOrbit)
		{
			// Make them orbit around the center of the screen.
			Planets[i].Acceleration = centerToPlanet.Normalized() * AttractionPower;

			if (Planets[i].Velocity.Length() > MaxVelocity)
			{
				Planets[i].Velocity = Planets[i].Velocity.Normalized() * MaxVelocity;
			}
		}
		else
		{
			// MCU orbit = v^2 / r = Planets[i].Velocity^2 / centerToPlanet.Length()
			Planets[i].Acceleration = centerToPlanet.Normalized() * (Planets[i].Velocity.Length() * Planets[i].Velocity.Length() / centerToPlanet.Length());
		}
	}
}

void Render()
{
	for (std::size_t i = 0; i < PlanetsCount; i++)
	{
		if (Planets[i].Mass == 1)
		{
			SDL_SetRenderDrawColor(gRenderer, 100, 100, 200, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(gRenderer, 200, 100, 100, 255);
		}

		DrawCircle(Planets[i].Position.X, Planets[i].Position.Y);
	}
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
        // Hack to get window to stay up
        SDL_Event e;
        bool quit = false;

        while(!quit)
        {
			Update();

			// Clear screen
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderClear(gRenderer);

			Render();

			SDL_RenderPresent(gRenderer);

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
