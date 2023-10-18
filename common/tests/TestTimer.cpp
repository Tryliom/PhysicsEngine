#include "Timer.h"

#include "gtest/gtest.h"

TEST(Timer, Init)
{
	Timer timer;

	EXPECT_FLOAT_EQ(timer.DeltaTime(), 0.0f);
}

TEST(Timer, Update)
{
    const float epsilon = 0.001f;
	Timer timer;

	auto time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaTime = std::chrono::duration<float>(0);

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.Update();

	deltaTime = std::chrono::high_resolution_clock::now() - time;

	EXPECT_NEAR(timer.DeltaTime(), deltaTime.count(), epsilon);
}