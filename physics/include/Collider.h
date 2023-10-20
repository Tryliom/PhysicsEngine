#pragma once

#include "Ref.h"
#include "ContactListener.h"
#include "Shape.h"

namespace Physics
{
	struct Shape
	{
		Math::CircleF Circle = Math::CircleF({0.f, 0.f}, 0.f);
		Math::RectangleF Rectangle = Math::RectangleF({0.f, 0.f}, {0.f, 0.f});
		Math::PolygonF Polygon = Math::PolygonF({{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}});
	};

	class Collider
	{
	public:
		constexpr Collider() noexcept = default;

	private:
		ColliderRef _colliderRef {};
		BodyRef _bodyRef {};
		ContactListener* _contactListener { nullptr };

		Math::ShapeType _shapeType { Math::ShapeType::Circle };
		Shape _shape {};

		float _bounciness { 0.f };
		float _friction { 0.f };
		bool _isTrigger { false };

		bool _isEnabled { false };
		bool _isFree { true };

	public:
		[[nodiscard]] ColliderRef GetColliderRef() const noexcept;
		[[nodiscard]] BodyRef GetBodyRef() const noexcept;
		[[nodiscard]] ContactListener* GetContactListener() const noexcept;
		[[nodiscard]] float GetBounciness() const noexcept;
		[[nodiscard]] float GetFriction() const noexcept;
		[[nodiscard]] bool IsTrigger() const noexcept;

		/**
		 * @brief Check if the collider is free
		 * @return true if the collider is free
		 */
		[[nodiscard]] bool IsFree() const noexcept;
		/**
		 * @brief Check if the collider is enabled (not free but disabled at this moment)
		 * @return true if the collider is enabled
		 */
		[[nodiscard]] bool IsEnabled() const noexcept;

		void SetColliderRef(ColliderRef colliderRef) noexcept;
		void SetBodyRef(BodyRef bodyRef) noexcept;
		void SetContactListener(ContactListener* contactListener) noexcept;
		void SetBounciness(float bounciness) noexcept;
		void SetFriction(float friction) noexcept;
		void SetIsTrigger(bool isTrigger) noexcept;

		void SetCircle(Math::CircleF circle) noexcept;
		void SetRectangle(Math::RectangleF rectangle) noexcept;
		void SetPolygon(Math::PolygonF polygon) noexcept;

		void Enable() noexcept;
		void Disable() noexcept;
		/**
		 * @brief Free the collider (reset its values)
		 */
		void Free() noexcept;

		[[nodiscard]] Math::ShapeType GetShapeType() const noexcept;
		[[nodiscard]] Math::CircleF GetCircle() const noexcept;
		[[nodiscard]] Math::RectangleF GetRectangle() const noexcept;
		[[nodiscard]] Math::PolygonF GetPolygon() const noexcept;
	};
}