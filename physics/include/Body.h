#pragma once

#include "Vec2.h"

namespace Physics
{
	class Body
	{
	public:
		constexpr Body() noexcept = default;
		Body(Math::Vec2F position, Math::Vec2F velocity) noexcept;

	private:
		Math::Vec2F _position = Math::Vec2F(0, 0);
		Math::Vec2F _velocity = Math::Vec2F(0, 0);
		Math::Vec2F _force = Math::Vec2F(0, 0);
		float _mass = -1.f;

	public:
		[[nodiscard]] Math::Vec2F Position() const noexcept;
		[[nodiscard]] Math::Vec2F Velocity() const noexcept;
		[[nodiscard]] Math::Vec2F Force() const noexcept;
		[[nodiscard]] float Mass() const noexcept;

		void SetPosition(Math::Vec2F position) noexcept;
		void SetVelocity(Math::Vec2F velocity) noexcept;
		void SetForce(Math::Vec2F force) noexcept;
		/**
		 * @brief Set the mass of the body. If the mass is less than or equal to 0, the mass will not be set.
		 * @note The mass is set to -1 by default, which means the body is disabled.
		 * @param mass
		 */
		void SetMass(float mass) noexcept;

		void ApplyForce(Math::Vec2F force) noexcept;
		/**
		 * @brief Disable the body by setting its mass to -1. Reset all other values.
		 */
		void Disable() noexcept;

		/**
		 * @brief Enable the body by setting its mass to 1.
		 */
		void Enable() noexcept;

		[[nodiscard]] bool IsEnabled() const noexcept;
	};
}