#pragma once

#include "Vec2.h"

class Body
{
public:
	Body() noexcept = default;

	Body(Vec2F position, Vec2F velocity, Vec2F acceleration) noexcept
	: Position(position), Velocity(velocity), Acceleration(acceleration) {}

	Vec2F Position = Vec2F(0, 0);
	Vec2F Velocity = Vec2F(0, 0);
	Vec2F Acceleration = Vec2F(0, 0);

private:
	float _mass = -1.f;

public:
	[[nodiscard]] constexpr float GetMass() const noexcept
	{
		return _mass;
	}

	void SetMass(float mass) noexcept
	{
		if (mass < 0.f) return;

		_mass = mass;
	}

	void Disable() noexcept
	{
		_mass = -1.f;

		Position = Vec2F(0, 0);
		Velocity = Vec2F(0, 0);
		Acceleration = Vec2F(0, 0);
	}

	void Enable() noexcept
	{
		_mass = 1.f;
	}

	[[nodiscard]] constexpr bool IsEnabled() const noexcept
	{
		return _mass >= 0.f;
	}
};