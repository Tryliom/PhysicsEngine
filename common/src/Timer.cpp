#include "Timer.h"

#include <chrono>

Timer::Timer() noexcept
{
    _currentTime = std::chrono::high_resolution_clock::now();
}

void Timer::Update() noexcept
{
    const auto previousTime = _currentTime;
    _currentTime = std::chrono::high_resolution_clock::now();
    _deltaTime = _currentTime - previousTime;
}

float Timer::DeltaTime() noexcept
{
    return _deltaTime.count();
}
