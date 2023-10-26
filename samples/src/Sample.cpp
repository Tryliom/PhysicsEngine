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
    Display::SetCameraPosition(Math::Vec2F(0.f, 0.f));
    Display::SetCameraZoom(1.f, Math::Vec2F::Zero());
    Display::SetMeterPerPixel(1.f);

    onInit();
}

void Sample::Deinit() noexcept
{
    onDeinit();
    _world = Physics::World();
}