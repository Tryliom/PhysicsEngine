#pragma once

#include <string>

void Update(float deltaTime);

namespace Display
{
	void Init(size_t width, size_t height, const std::string& name = "Display") noexcept;
	/**
	 * @brief Call Update() in a loop until the user closes the window.
	 */
	void Run() noexcept;
	void Shutdown() noexcept;

	size_t GetWidth() noexcept;
	size_t GetHeight() noexcept;

	void DrawCircle(int x, int y, int radius) noexcept;
}