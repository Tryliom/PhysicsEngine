#include "Timer.h"

#include "gtest/gtest.h"

using namespace Physics;

TEST(Timer, Init)
{
	Timer::Init();

	EXPECT_FLOAT_EQ(Timer::DeltaTime(), 0.0f);
}

TEST(Timer, Update)
{
	Timer::Init();
	auto time = std::chrono::high_resolution_clock::now().time_since_epoch();

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	Timer::Update();

	time = std::chrono::high_resolution_clock::now().time_since_epoch() - time;

	EXPECT_FLOAT_EQ(Timer::DeltaTime(), time.count() / 1000000000.f);
}