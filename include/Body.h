#pragma once

#include "../libs/Math922/include/Vec2.h"

class Body
{
public:
	Vec2F Position = Vec2F(0, 0);
	Vec2F Velocity = Vec2F(0, 0);
	Vec2F Acceleration = Vec2F(0, 0);
	float Mass { 0 };

	Body() noexcept = default;

	Body(Vec2F position, Vec2F velocity, Vec2F acceleration, float mass) noexcept
	: Position(position), Velocity(velocity), Acceleration(acceleration), Mass(mass) {}

	void Update(float deltaTime) noexcept
	{
		Velocity += Acceleration * deltaTime;
		Position += Velocity * deltaTime;
	}
};