#include "Body.h"
#include "Vec2.h"

#include "gtest/gtest.h"

#include <array>

struct TestBodyFixtureConstructor : public ::testing::TestWithParam<std::array<Vec2F, 3>> {};

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