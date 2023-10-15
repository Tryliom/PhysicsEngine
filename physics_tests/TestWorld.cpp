#include "World.h"
#include "Body.h"
#include "Exception.h"

#include "gtest/gtest.h"

#include <array>

struct TestWorldFixtureTime : public ::testing::TestWithParam<std::pair<std::array<Vec2F, 4>, std::pair<float, float>>> {};

INSTANTIATE_TEST_SUITE_P(World, TestWorldFixtureTime, testing::Values(
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0, 0), Vec2F(1, 1), Vec2F(1, 1), Vec2F(3.f, 0.34f)}, std::make_pair(1.3f, 1.f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(1.9f, 20.0f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(0.3244f, 100.4f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(0.94f, 0.3244f))
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
	auto deltaTime = pair.second.first;
	auto bodyRef = World::CreateBody();
	auto& body = World::GetBody(bodyRef);

	body.SetPosition(pair.first[0]);
	body.SetVelocity(pair.first[1]);
	body.SetAcceleration(pair.first[2]);
	body.SetMass(pair.second.second);
	body.ApplyForce(pair.first[3]);

	auto acceleration = body.Force() / body.Mass();
	auto velocity = body.Velocity() + acceleration * deltaTime;
	auto position = body.Position() + velocity * deltaTime;

	World::Update(deltaTime);

	EXPECT_FLOAT_EQ(body.Position().X, position.X);
	EXPECT_FLOAT_EQ(body.Position().Y, position.Y);

	EXPECT_FLOAT_EQ(body.Velocity().X, velocity.X);
	EXPECT_FLOAT_EQ(body.Velocity().Y, velocity.Y);

	EXPECT_FLOAT_EQ(body.Acceleration().X, acceleration.X);
	EXPECT_FLOAT_EQ(body.Acceleration().Y, acceleration.Y);

	EXPECT_FLOAT_EQ(body.Force().X, 0.f);
	EXPECT_FLOAT_EQ(body.Force().Y, 0.f);
}