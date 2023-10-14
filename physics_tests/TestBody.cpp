#include "gtest/gtest.h"
#include "Vec2.h"
#include "../common/include/Body.h"

#include <array>

struct TestBodyFixtureConstructor : public ::testing::TestWithParam<std::array<Vec2F, 3>> {};
struct TestBodyFixtureWithTime : public ::testing::TestWithParam<std::pair<std::array<Vec2F, 3>, float>> {};

INSTANTIATE_TEST_SUITE_P(Body, TestBodyFixtureConstructor, testing::Values(
	std::array<Vec2F, 3>{
		Vec2F(-5.0f, 2.0f), // position
		Vec2F(0.4f, 1.0f), // velocity
		Vec2F(1.2f, -1.4f) // acceleration
	},
	std::array<Vec2F, 3>{
		Vec2F(0.0f, 0.0f), // position
		Vec2F(0.0f, 0.0f), // velocity
		Vec2F(0.0f, 0.0f) // acceleration
	}
));

INSTANTIATE_TEST_SUITE_P(Body, TestBodyFixtureWithTime, testing::Values(
	std::pair<std::array<Vec2F, 3>, float>{
		std::array<Vec2F, 3>{
			Vec2F(-5.0f, 2.0f), // position
			Vec2F(0.4f, 1.0f), // velocity
			Vec2F(1.2f, -1.4f) // acceleration
		},
		0.74f
	},
	std::pair<std::array<Vec2F, 3>, float>{
		std::array<Vec2F, 3>{
			Vec2F(0.0f, 0.0f), // position
			Vec2F(0.0f, 0.0f), // velocity
			Vec2F(0.0f, 0.0f) // acceleration
		},
		1.93f
	}
));

TEST_P(TestBodyFixtureConstructor, Constructor)
{
	auto params = GetParam();
	Vec2F position = params[0];
	Vec2F velocity = params[1];
	Vec2F acceleration = params[2];

	Body body(position, velocity, acceleration);

	EXPECT_EQ(body.Position, position);
	EXPECT_EQ(body.Velocity, velocity);
	EXPECT_EQ(body.Acceleration, acceleration);
}

TEST_P(TestBodyFixtureWithTime, Update)
{
	auto pair = GetParam();
	auto params = pair.first;
	Vec2F position = params[0];
	Vec2F velocity = params[1];
	Vec2F acceleration = params[2];
	float deltaTime = pair.second;

	Body body(position, velocity, acceleration);

	body.Update(deltaTime);

	velocity += acceleration * deltaTime;
	position += velocity * deltaTime;

	EXPECT_EQ(body.Position, position);
	EXPECT_EQ(body.Velocity, velocity);
}