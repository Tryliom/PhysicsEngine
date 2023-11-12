#include "Sample.h"

#include "Display.h"
#include "Timer.h"

#include <utility>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

Sample::Sample(std::string name, std::string description) noexcept
{
	_name = std::move(name);
	_description = std::move(description);
}

void Sample::Update(float deltaTime) noexcept
{
#ifdef TRACY_ENABLE
	ZoneNamedN(sampleUpdate, "Sample::Update", true);
#endif

    _world.Update(deltaTime);

	if (!Display::IsMouseOnAnImGuiWindow())
	{
		onInput();
	}

    onUpdate(deltaTime);
}

void Sample::Render() noexcept
{
    onRender();
}

void Sample::Init() noexcept
{
    Display::ResetView();
    onInit();
}

void Sample::Deinit() noexcept
{
    onDeinit();
    _world = Physics::World();
}

void Sample::DrawImGui() noexcept
{
    onDrawImGui();
}