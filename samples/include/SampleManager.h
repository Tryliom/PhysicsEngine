#pragma once

#include "Sample.h"

#include "UniquePtr.h"
#include "Timer.h"

#include <array>

class SampleManager
{
private:
    std::array<UniquePtr<Sample>, 2> _samples;
    std::size_t _currentSample = 0;
    Timer _timer;

	std::vector<std::string> _names;
	std::vector<std::string> _descriptions;

    static constexpr int SCREEN_WIDTH = 1550;
    static constexpr int SCREEN_HEIGHT = 900;

public:
    SampleManager() noexcept;
    ~SampleManager() = default;

    void Run() noexcept;

private:
    void switchSample(int sampleIndex) noexcept;
	void drawImGui() noexcept;
};