#pragma once

#include <chrono>

class Timer
{
public:
    Timer() noexcept;

private:
    std::chrono::duration<float> _deltaTime = std::chrono::duration<float>(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> _currentTime;

public:
	void Update() noexcept;

	/**
	 * @brief Get the Delta Time in seconds
	 */
	float DeltaTime() noexcept;
};