#include "TriggerSample.h"

#include "Display.h"
#include "Timer.h"
#include "Input.h"

int main(int argc, char* args[])
{
    constexpr int SCREEN_WIDTH = 1550;
    constexpr int SCREEN_HEIGHT = 900;

    Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "_planets");

	TriggerSample triggers;
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
	    triggers.Update(timer.DeltaTime());

        Display::ClearRender();
	    triggers.Render();
        Display::Render();
    }
}