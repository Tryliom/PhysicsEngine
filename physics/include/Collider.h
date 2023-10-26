#pragma once

#include "Shape.h"
#include "Ref.h"

#include <variant>

namespace Physics
{
    /**
     * @brief Collider class
     */
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
        /**
         * @brief Get the body reference of the collider
         * @return the body reference
         */
		[[nodiscard]] BodyRef GetBodyRef() const noexcept;
        /**
         * @brief Get the offset position of the collider
         * @return the offset position
         */
        [[nodiscard]] Math::Vec2F GetOffset() const noexcept;
        /**
         * @brief Get the bounciness of the collider
         * @return the bounciness
         */
		[[nodiscard]] float GetBounciness() const noexcept;
        /**
         * @brief Get the friction of the collider
         * @return the friction
         */
		[[nodiscard]] float GetFriction() const noexcept;
        /**
         * @brief Check if the collider is a trigger
         * @return True if the collider is a trigger
         */
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

        /**
         * @brief Set the body reference of the collider
         * @param bodyRef the body reference
         */
		void SetBodyRef(BodyRef bodyRef) noexcept;
        /**
         * @brief Set the offset position of the collider
         * @param offset the offset position
         */
        void SetOffset(Math::Vec2F offset) noexcept;
        /**
         * @brief Set the bounciness of the collider
         * @param bounciness the bounciness
         */
		void SetBounciness(float bounciness) noexcept;
        /**
         * @brief Set the friction of the collider
         * @param friction the friction
         */
		void SetFriction(float friction) noexcept;
        /**
         * @brief Set if the collider is a trigger
         * @param isTrigger true if the collider is a trigger
         */
		void SetIsTrigger(bool isTrigger) noexcept;

        /**
         * @brief Set the shape of the collider to a circle, the circle center is not used
         * @param circle the circle
         */
		void SetCircle(Math::CircleF circle) noexcept;
        /**
         * @brief Set the shape of the collider to a rectangle
         * @param rectangle the rectangle
         */
		void SetRectangle(Math::RectangleF rectangle) noexcept;
        /**
         * @brief Set the shape of the collider to a polygon
         * @param polygon the polygon
         */
		void SetPolygon(Math::PolygonF polygon) noexcept;

        /**
         * @brief Enable the collider
         */
		void Enable() noexcept;
        /**
         * @brief Disable the collider
         */
		void Disable() noexcept;
		/**
		 * @brief Free the collider (reset its values)
		 */
		void Free() noexcept;

        /**
         * @brief Get the shape type of the collider
         * @return the shape type
         */
		[[nodiscard]] Math::ShapeType GetShapeType() const noexcept;

        /**
         * @brief Get the circle of the collider
         * @return the circle
         */
		[[nodiscard]] Math::CircleF GetCircle() const noexcept;
        /**
         * @brief Get the rectangle of the collider
         * @return the rectangle
         */
        [[nodiscard]] Math::RectangleF GetRectangle() const noexcept;
        /**
         * @brief Get the polygon of the collider
         * @return the polygon
         */
		[[nodiscard]] Math::PolygonF GetPolygon() const noexcept;
	};
}