#pragma once

#include "Vec2.h"

//TODO: Make Body.cpp

namespace Physics
{
	class Body
	{
	public:
		Body() noexcept = default;

		Body(Math::Vec2F position, Math::Vec2F velocity) noexcept : _position(position), _velocity(velocity) {}

	private:
		Math::Vec2F _position = Math::Vec2F(0, 0);
		Math::Vec2F _velocity = Math::Vec2F(0, 0);
		Math::Vec2F _force = Math::Vec2F(0, 0);
		float _mass = -1.f;

	public:
		[[nodiscard]] constexpr Math::Vec2F Position() const noexcept
		{
			return _position;
		}

		void SetPosition(Math::Vec2F position) noexcept
		{
			_position = position;
		}

		[[nodiscard]] constexpr Math::Vec2F Velocity() const noexcept
		{
			return _velocity;
		}

		void SetVelocity(Math::Vec2F velocity) noexcept
		{
			_velocity = velocity;
		}

		[[nodiscard]] constexpr Math::Vec2F Force() const noexcept
		{
			return _force;
		}

		void SetForce(Math::Vec2F force) noexcept
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

		void ApplyForce(Math::Vec2F force) noexcept
		{
			_force += force;
		}

		/**
		 * @brief Disable the body by setting its mass to -1. Reset all other values.
		 */
		void Disable() noexcept
		{
			_mass = -1.f;

			_position = Math::Vec2F(0, 0);
			_velocity = Math::Vec2F(0, 0);
			_force = Math::Vec2F(0, 0);
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
}