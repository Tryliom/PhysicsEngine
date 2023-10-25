#pragma once

#include "Shape.h"
#include "Ref.h"

#include <variant>

namespace Physics
{
	class Collider
	{
	public:
		constexpr Collider() noexcept = default;

	private:
		BodyRef _bodyRef {};
		Math::ShapeType _shapeType { Math::ShapeType::Circle };
        std::variant<Math::CircleF, Math::RectangleF, Math::PolygonF> _shape { Math::CircleF(Math::Vec2F::Zero(), 1.f) };

        Math::Vec2F _offset { Math::Vec2F::Zero() };

		float _bounciness { 0.f };
		float _friction { 0.f };
		bool _isTrigger { false };

		bool _isEnabled { false };
		bool _isFree { true };

	public:
		[[nodiscard]] BodyRef GetBodyRef() const noexcept;
        [[nodiscard]] Math::Vec2F GetOffset() const noexcept;
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

		void SetBodyRef(BodyRef bodyRef) noexcept;
        void SetOffset(Math::Vec2F offset) noexcept;
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