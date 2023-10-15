#include "Timer.h"

#include <chrono>

namespace Timer
{
	std::chrono::nanoseconds _deltaTime = std::chrono::nanoseconds(0);
	std::chrono::nanoseconds _currentTime = std::chrono::nanoseconds(0);

	void Init() noexcept
	{
		_currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
	}

	void Update() noexcept
	{
		std::chrono::nanoseconds previousTime = _currentTime;
		_currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
		_deltaTime = _currentTime - previousTime;
	}

	float DeltaTime() noexcept
	{
		return _deltaTime.count() / 1000000000.f;
	}
}
