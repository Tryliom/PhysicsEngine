#include "PlanetSystem.h"

#include "Display.h"
#include "Timer.h"
#include "Input.h"

int main(int argc, char* args[])
{
    PlanetSystem planetSystem;
    Timer timer;

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

        Display::Update();
    }
}