#include "SampleManager.h"

#include "TriggerSample.h"
#include "PlanetSystemSample.h"
#include "Input.h"
#include "Display.h"

SampleManager::SampleManager() noexcept : _timer(), _samples({
    MakeUnique<Sample, TriggerSample>(),
    MakeUnique<Sample, PlanetSystemSample>()
})
{
    Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Samples selection");

    _samples[_currentSample]->Init();
}

void SampleManager::Run() noexcept
{
    while (true)
    {
        Input::Update();

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT) return;

            Input::OnInput(e);

            if (Input::IsKeyPressed(SDL_SCANCODE_LEFT)) switchSample(static_cast<int>(_currentSample) - 1);
            if (Input::IsKeyPressed(SDL_SCANCODE_RIGHT)) switchSample(static_cast<int>(_currentSample) + 1);
        }

        _timer.Update();
        _samples[_currentSample]->Update(_timer.DeltaTime());

        Display::ClearRender();
        _samples[_currentSample]->Render();
        Display::Render();
    }
}

void SampleManager::switchSample(int sampleIndex) noexcept
{
    if (sampleIndex < 0 || sampleIndex >= _samples.size()) return;

    _samples[_currentSample]->Deinit();
    _currentSample = sampleIndex;
    _samples[_currentSample]->Init();
}