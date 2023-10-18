#pragma once

#include "Vec2.h"

#include <string>
#include <SDL_events.h>

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
	Math::Vec2F Position { 0.f, 0.f };
	float Zoom { 1.f };
};

namespace Display
{
	void Init(size_t width, size_t height, const std::string& name = "Display") noexcept;
	void Render() noexcept;
	void Shutdown() noexcept;

	size_t GetWidth() noexcept;
	size_t GetHeight() noexcept;

    void SetUpdate(void(*update)(float)) noexcept;
	void SetMeterPerPixel(float meterPerPixel) noexcept;
	Math::Vec2F GetMousePosition() noexcept;
	Math::Vec2F GetMouseDelta() noexcept;
	Math::Vec2F GetCameraPosition() noexcept;

	void MoveCamera(Math::Vec2F delta) noexcept;
	void SetCameraPosition(Math::Vec2F position) noexcept;
	void LookAt(Math::Vec2F position) noexcept;
	void SetCameraZoom(float zoom, Math::Vec2F targetPoint) noexcept;
	float GetCameraZoom() noexcept;

    void ClearRender() noexcept;
	void DrawCircle(float x, float y, float radius, Color color = Color::Red()) noexcept;
}