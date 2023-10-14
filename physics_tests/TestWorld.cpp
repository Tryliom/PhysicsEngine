#include "World.h"
#include "Body.h"
#include "Exception.h"

#include "gtest/gtest.h"

#include <array>

struct TestWorldFixtureTime : public ::testing::TestWithParam<std::pair<std::array<Vec2F, 3>, float>> {};

INSTANTIATE_TEST_SUITE_P(World, TestWorldFixtureTime, testing::Values(
	std::make_pair(std::array<Vec2F, 3>{ Vec2F(0, 0), Vec2F(1, 1), Vec2F(1, 1) }, 1.3f),
	std::make_pair(std::array<Vec2F, 3>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, 0.54f),
	std::make_pair(std::array<Vec2F, 3>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, 0.0f),
	std::make_pair(std::array<Vec2F, 3>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, 0.3244f)
));

TEST(World, CreateBody)
{
	World::Init(1);

	auto body = World::CreateBody();

	EXPECT_EQ(body.Index, 0);
	EXPECT_EQ(body.Generation, 0);
	EXPECT_TRUE(World::GetBody(body).IsEnabled());

	World::DestroyBody(body);

	body = World::CreateBody();

	EXPECT_EQ(body.Index, 0);
	EXPECT_EQ(body.Generation, 1);
	EXPECT_TRUE(World::GetBody(body).IsEnabled());

	auto body2 = World::CreateBody();

	EXPECT_EQ(body2.Index, 1);
	EXPECT_EQ(body2.Generation, 0);
	EXPECT_TRUE(World::GetBody(body2).IsEnabled());

	World::DestroyBody(body);

	EXPECT_THROW(World::GetBody(body), InvalidBodyRefException);
}

TEST_P(TestWorldFixtureTime, Update)
{
	World::Init(1);

	auto pair = GetParam();
	auto deltaTime = pair.second;
	auto bodyRef = World::CreateBody();
	auto& body = World::GetBody(bodyRef);

	body.Position = pair.first[0];
	body.Velocity = pair.first[1];
	body.Acceleration = pair.first[2];

	auto velocity = body.Acceleration * deltaTime + body.Velocity;
	auto position = velocity * deltaTime + body.Position;

	World::Update(deltaTime);

	EXPECT_FLOAT_EQ(body.Position.X, position.X);
	EXPECT_FLOAT_EQ(body.Position.Y, position.Y);

	EXPECT_FLOAT_EQ(body.Velocity.X, velocity.X);
	EXPECT_FLOAT_EQ(body.Velocity.Y, velocity.Y);

	EXPECT_FLOAT_EQ(body.Acceleration.X, pair.first[2].X);
	EXPECT_FLOAT_EQ(body.Acceleration.Y, pair.first[2].Y);
}