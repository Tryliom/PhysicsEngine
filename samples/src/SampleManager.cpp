#include "SampleManager.h"

#include "TriggerSample.h"
#include "PlanetSystemSample.h"
#include "CollisionSample.h"
#include "GravitySample.h"
#include "Input.h"
#include "Display.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

SampleManager::SampleManager() noexcept : _timer(), _samples({
    MakeUnique<Sample, TriggerSample>(),
	MakeUnique<Sample, CollisionSample>(),
    MakeUnique<Sample, GravitySample>(),
    MakeUnique<Sample, PlanetSystemSample>()
})
{
    Display::Init(SCREEN_WIDTH, SCREEN_HEIGHT, "Samples selection");

    _samples[_currentSample]->Init();

	_names.reserve(_samples.size());
	_descriptions.reserve(_samples.size());

	for (const auto& _sample : _samples)
	{
		_names.push_back(_sample->GetName());
		_descriptions.push_back(_sample->GetDescription());
	}
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

			ImGui_ImplSDL2_ProcessEvent(&e);
            Input::OnInput(e);
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        _timer.Update();
        _samples[_currentSample]->Update(_timer.DeltaTime());

        Display::ClearRender();
		drawImGui();
        _samples[_currentSample]->Render();
	    Display::Render();

#ifdef TRACY_ENABLE
	    FrameMark;
#endif
	}
}

void SampleManager::switchSample(int sampleIndex) noexcept
{
    if (sampleIndex < 0 || sampleIndex >= _samples.size()) return;

    _samples[_currentSample]->Deinit();
    _currentSample = sampleIndex;
    _samples[_currentSample]->Init();
}

void SampleManager::drawImGui() noexcept
{
	ImGui::Begin("Sample selector");

	// Create a combo box with all the samples
	if (ImGui::BeginCombo("Samples", _names[_currentSample].c_str()))
	{
		for (int i = 0; i < _names.size(); i++)
		{
			bool isSelected = (_currentSample == i);

			if (ImGui::Selectable(_names[i].c_str(), isSelected))
			{
				switchSample(i);
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Spacing();

	// Display the description of the current sample
	ImGui::TextWrapped("%s", _descriptions[_currentSample].c_str());
    ImGui::Spacing();

    _samples[_currentSample]->DrawImGui();

	ImGui::End();
}