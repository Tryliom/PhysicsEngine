#pragma once

#include "Vec2.h"

namespace Physics
{
    /**
     * @brief A body is a point in space with a velocity and a mass that can be affected by forces.
     */
	class Body
	{
	public:
		constexpr Body() noexcept = default;
        /**
         * @brief Construct a new Body object
         * @param position The position of the body
         * @param velocity The velocity of the body
         */
		Body(Math::Vec2F position, Math::Vec2F velocity) noexcept;

	private:
		Math::Vec2F _position = Math::Vec2F(0, 0);
		Math::Vec2F _velocity = Math::Vec2F(0, 0);
		Math::Vec2F _force = Math::Vec2F(0, 0);
		float _mass = -1.f;

	public:
        /**
         * @brief Get the position of the body
         * @return The position of the body
         */
		[[nodiscard]] Math::Vec2F Position() const noexcept;
        /**
         * @brief Get the velocity of the body
         * @return The velocity of the body
         */
		[[nodiscard]] Math::Vec2F Velocity() const noexcept;
        /**
         * @brief Get the force of the body
         * @return The force of the body
         */
		[[nodiscard]] Math::Vec2F Force() const noexcept;
        /**
         * @brief Get the mass of the body
         * @return The mass of the body
         */
		[[nodiscard]] float Mass() const noexcept;

        /**
         * @brief Set the position of the body
         * @param position The new position of the body
         */
		void SetPosition(Math::Vec2F position) noexcept;
        /**
         * @brief Set the velocity of the body
         * @param velocity The new velocity of the body
         */
		void SetVelocity(Math::Vec2F velocity) noexcept;
        /**
         * @brief Set the force of the body
         * @param force The new force of the body
         */
		void SetForce(Math::Vec2F force) noexcept;
		/**
		 * @brief Set the mass of the body. If the mass is less than or equal to 0, the mass will not be set.
		 * @note The mass is set to -1 by default, which means the body is disabled.
		 * @param mass
		 */
		void SetMass(float mass) noexcept;

        /**
         * @brief Apply a force to the body (add it to the current force)
         * @param force The force to apply
         */
		void ApplyForce(Math::Vec2F force) noexcept;
		/**
		 * @brief Disable the body by setting its mass to -1. Clear all other values.
		 */
		void Disable() noexcept;

		/**
		 * @brief Enable the body by setting its mass to 1.
		 */
		void Enable() noexcept;

        /**
         * @brief Check if the body is enabled
         * @return true if the body is enabled
         */
		[[nodiscard]] bool IsEnabled() const noexcept;
	};
}