#include "PlanetSystem.h"

#include "Display.h"
#include "Timer.h"
#include "Input.h"

int main(int argc, char* args[])
{
    PlanetSystem planetSystem;
    Timer timer;

    constexpr int SCREEN_WIDTH = 1550;
    constexpr int SCREEN_HEIGHT = 900;

    Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "_planets");

    while (true)
    {
        Input::Update();

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT) return EXIT_SUCCESS;

            Input::OnInput(e);
        }

        timer.Update();
        planetSystem.Update(timer.DeltaTime());

        Display::ClearRender();
        planetSystem.Render();
        Display::Render();
    }
}