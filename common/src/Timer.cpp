#include "Timer.h"

#include <chrono>

std::chrono::nanoseconds deltaTime = std::chrono::nanoseconds(0);
std::chrono::nanoseconds currentTime = std::chrono::nanoseconds(0);
std::chrono::nanoseconds previousTime = std::chrono::nanoseconds(0);

namespace Timer
{
	void Init() noexcept
	{
		currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
		previousTime = currentTime;
	}

	void Update() noexcept
	{
		previousTime = currentTime;
		currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
		deltaTime = currentTime - previousTime;
	}

	float DeltaTime() noexcept
	{
		return deltaTime.count() / 1000000000.f;
	}
}
