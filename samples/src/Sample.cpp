#include "Sample.h"

#include "Display.h"
#include "Timer.h"

void Sample::Update(float deltaTime) noexcept
{
    _world.Update(deltaTime);
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
    _world = Physics::World();
}