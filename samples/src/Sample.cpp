#include "Sample.h"

#include <utility>

#include "Display.h"
#include "Timer.h"

Sample::Sample(std::string name, std::string description) noexcept
{
	_name = std::move(name);
	_description = std::move(description);
}

void Sample::Update(float deltaTime) noexcept
{
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
    Display::Render();
}

void Sample::Init() noexcept
{
    Display::ResetView();
    onInit();
}

void Sample::Deinit() noexcept
{
    onDeinit();
    _world = Physics::World(1);
}