#pragma once

#include "Vec2.h"

class Body
{
public:
	Body() noexcept = default;

	Body(Vec2F position, Vec2F velocity, Vec2F acceleration) noexcept
	: _position(position), _velocity(velocity), _acceleration(acceleration) {}

private:
	Vec2F _position = Vec2F(0, 0);
	Vec2F _velocity = Vec2F(0, 0);
	Vec2F _acceleration = Vec2F(0, 0);
	Vec2F _force = Vec2F(0, 0);
	float _mass = -1.f;

public:
	[[nodiscard]] constexpr Vec2F Position() const noexcept
	{
		return _position;
	}

	void SetPosition(Vec2F position) noexcept
	{
		_position = position;
	}

	[[nodiscard]] constexpr Vec2F Velocity() const noexcept
	{
		return _velocity;
	}

	void SetVelocity(Vec2F velocity) noexcept
	{
		_velocity = velocity;
	}

	[[nodiscard]] constexpr Vec2F Acceleration() const noexcept
	{
		return _acceleration;
	}

	void SetAcceleration(Vec2F acceleration) noexcept
	{
		_acceleration = acceleration;
	}

	[[nodiscard]] constexpr Vec2F Force() const noexcept
	{
		return _force;
	}

	void SetForce(Vec2F force) noexcept
	{
		_force = force;
	}

	[[nodiscard]] constexpr float Mass() const noexcept
	{
		return _mass;
	}

	/**
	 * @brief Set the mass of the body. If the mass is less than or equal to 0, the mass will not be set.
	 * @note The mass is set to -1 by default, which means the body is disabled.
	 * @param mass
	 */
	void SetMass(float mass) noexcept
	{
		if (mass <= 0.f) return;

		_mass = mass;
	}

	void ApplyForce(Vec2F force) noexcept
	{
		_force += force * _mass;
	}

	/**
	 * @brief Disable the body by setting its mass to -1. Reset all other values.
	 */
	void Disable() noexcept
	{
		_mass = -1.f;

		_position = Vec2F(0, 0);
		_velocity = Vec2F(0, 0);
		_acceleration = Vec2F(0, 0);
		_force = Vec2F(0, 0);
	}

	/**
	 * @brief Enable the body by setting its mass to 1.
	 */
	void Enable() noexcept
	{
		_mass = 1.f;
	}

	[[nodiscard]] constexpr bool IsEnabled() const noexcept
	{
		return _mass > 0.f;
	}
};