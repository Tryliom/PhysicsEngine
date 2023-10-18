#include "World.h"
#include "Body.h"
#include "Exception.h"

#include "gtest/gtest.h"

#include <array>

using namespace Physics;
using namespace Math;

struct TestWorldFixtureTime : public ::testing::TestWithParam<std::pair<std::array<Vec2F, 4>, std::pair<float, float>>> {};

INSTANTIATE_TEST_SUITE_P(World, TestWorldFixtureTime, testing::Values(
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0, 0), Vec2F(1, 1), Vec2F(1, 1), Vec2F(3.f, 0.34f)}, std::make_pair(1.3f, 1.f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(1.9f, 20.0f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(0.3244f, 100.4f)),
	std::make_pair(std::array<Vec2F, 4>{ Vec2F(0.3f, 1.3f), Vec2F(0.634f, 1.f), Vec2F(0.6f, 1.6f) }, std::make_pair(0.94f, 0.3244f))
));

TEST(World, CreateBody)
{
	World world(1);

	auto body = world.CreateBody();

	EXPECT_EQ(body.Index, 0);
	EXPECT_EQ(body.Generation, 0);
	EXPECT_TRUE(world.GetBody(body).IsEnabled());

	world.DestroyBody(body);

	body = world.CreateBody();

	EXPECT_EQ(body.Index, 0);
	EXPECT_EQ(body.Generation, 1);
	EXPECT_TRUE(world.GetBody(body).IsEnabled());

	auto body2 = world.CreateBody();

	EXPECT_EQ(body2.Index, 1);
	EXPECT_EQ(body2.Generation, 0);
	EXPECT_TRUE(world.GetBody(body2).IsEnabled());

	world.DestroyBody(body);

	EXPECT_THROW(world.GetBody(body), InvalidBodyRefException);
}

TEST_P(TestWorldFixtureTime, Update)
{
    World world(1);

	auto pair = GetParam();
	auto deltaTime = pair.second.first;
	auto bodyRef = world.CreateBody();
	auto& body = world.GetBody(bodyRef);

	body.SetPosition(pair.first[0]);
	body.SetVelocity(pair.first[1]);
	body.SetMass(pair.second.second);
	body.ApplyForce(pair.first[3] * body.Mass());

	auto velocity = body.Velocity() + body.Force() * deltaTime;
	auto position = body.Position() + velocity * deltaTime;

	world.Update(deltaTime);

	EXPECT_FLOAT_EQ(body.Position().X, position.X);
	EXPECT_FLOAT_EQ(body.Position().Y, position.Y);

	EXPECT_FLOAT_EQ(body.Velocity().X, velocity.X);
	EXPECT_FLOAT_EQ(body.Velocity().Y, velocity.Y);

	EXPECT_FLOAT_EQ(body.Force().X, 0.f);
	EXPECT_FLOAT_EQ(body.Force().Y, 0.f);
}