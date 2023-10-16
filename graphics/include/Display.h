#pragma once

#include "Vec2.h"

#include <string>
#include <SDL_events.h>

void Update(float deltaTime);

struct Color
{
	unsigned char R { 0 };
	unsigned char G { 0 };
	unsigned char B { 0 };
	unsigned char A { 255 };

	static constexpr Color Red() noexcept { return Color(255, 0, 0); }
	static constexpr Color Green() noexcept { return Color(0, 255, 0); }
	static constexpr Color Blue() noexcept { return Color(0, 0, 255); }
	static constexpr Color White() noexcept { return Color(255, 255, 255); }
	static constexpr Color Black() noexcept { return Color(0, 0, 0); }
};

struct Camera
{
	Vec2F Position { 0.f, 0.f };
	float Zoom { 1.f };
};

namespace Display
{
	void Init(size_t width, size_t height, const std::string& name = "Display") noexcept;
	/**
	 * @brief Call Update() in a loop until the user closes the window. Call OnInput() for each SDL_Event.
	 */
	void Run() noexcept;
	void Shutdown() noexcept;

	size_t GetWidth() noexcept;
	size_t GetHeight() noexcept;

	void SetMeterPerPixel(float meterPerPixel) noexcept;
	Vec2F GetMousePosition() noexcept;
	Vec2F GetMouseDelta() noexcept;

	void MoveCamera(Vec2F delta) noexcept;
	void SetCameraPosition(Vec2F position) noexcept;
	void SetCameraZoom(float zoom) noexcept;
	float GetCameraZoom() noexcept;

	void PushColor(Color color) noexcept;
	void DrawCircle(int x, int y, int radius) noexcept;
}