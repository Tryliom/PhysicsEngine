#include "Sample.h"

#include "Display.h"
#include "Timer.h"

#include "imgui.h"

void Sample::Update(float deltaTime) noexcept
{
    _world.Update(deltaTime);
    onUpdate(deltaTime);
}

void Sample::Render() noexcept
{
    onRender();
    DrawImGui();
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

void Sample::DrawImGui() noexcept
{
    ImGui::Begin("Sample");
    ImGui::Text("tuitu");
    ImGui::End();
}