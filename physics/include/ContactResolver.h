#pragma once

#include "Body.h"
#include "Collider.h"

namespace Physics
{
	class ContactResolver
	{
	public:
		ContactResolver(Body* bodyA, Body* bodyB, Collider* colliderA, Collider* colliderB) noexcept;
		~ContactResolver() noexcept = default;

	private:
		Body* _bodyA {nullptr };
		Body* _bodyB {nullptr };
		Collider* _colliderA { nullptr };
		Collider* _colliderB { nullptr };

		Math::Vec2F _normal { 0.0f, 0.0f };
		float _penetration { 0.0f };

	public:
		/**
		 * @brief Calculate the contact between the two colliders, applying the correct forces to the bodies and resolving the collision
		 */
		void ResolveContact() noexcept;

	private:
		/**
		 * @brief Setup the contact between the two colliders
		 */
		void setupContact() noexcept;
		/**
		 * @brief Resolve the collision between the two colliders
		 */
		void resolveCollision() noexcept;
		/**
		 * @brief Resolve the position of the two colliders
		 */
		void resolvePosition() noexcept;

		/**
		 * @brief Resolve the collision between two circles
		 */
		void resolveCircleToCircle() noexcept;
		/**
		 * @brief Resolve the collision between two rectangles
		 */
		void resolveRectangleToRectangle() noexcept;
		/**
		 * @brief Resolve the collision between a circle and a rectangle
		 */
		void resolveCircleToRectangle() noexcept;
	};
}