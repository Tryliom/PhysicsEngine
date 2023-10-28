#include "Collider.h"

namespace Physics
{
	BodyRef Collider::GetBodyRef() const noexcept
	{
		return _bodyRef;
	}

    Math::Vec2F Collider::GetOffset() const noexcept
    {
        return _offset;
    }

    float Collider::GetBounciness() const noexcept
	{
		return _bounciness;
	}

	float Collider::GetFriction() const noexcept
	{
		return _friction;
	}

	bool Collider::IsTrigger() const noexcept
	{
		return _isTrigger;
	}

	bool Collider::IsFree() const noexcept
	{
		return _shapeType == Math::ShapeType::None;
	}

	bool Collider::IsEnabled() const noexcept
	{
		return !IsFree() && _isEnabled;
	}

	void Collider::SetBodyRef(BodyRef bodyRef) noexcept
	{
		_bodyRef = bodyRef;
	}


    void Collider::SetOffset(Math::Vec2F offset) noexcept
    {
        _offset = offset;
    }

	void Collider::SetBounciness(float bounciness) noexcept
	{
		_bounciness = bounciness;
	}

	void Collider::SetFriction(float friction) noexcept
	{
		_friction = friction;
	}

	void Collider::SetIsTrigger(bool isTrigger) noexcept
	{
		_isTrigger = isTrigger;
	}

	void Collider::SetCircle(Math::CircleF circle) noexcept
	{
		_shapeType = Math::ShapeType::Circle;
		_shape = circle;
	}

	void Collider::SetRectangle(Math::RectangleF rectangle) noexcept
	{
		_shapeType = Math::ShapeType::Rectangle;
		_shape = rectangle;
	}

	void Collider::SetPolygon(Math::PolygonF polygon) noexcept
	{
		_shapeType = Math::ShapeType::Polygon;
		_shape = polygon;
	}

	void Collider::Enable() noexcept
	{
		_isEnabled = true;
	}

	void Collider::Disable() noexcept
	{
		_isEnabled = false;
	}

	void Collider::Free() noexcept
	{
		_isEnabled = false;
		_bounciness = 0.f;
		_friction = 0.f;
		_isTrigger = false;
		_shapeType = Math::ShapeType::None;
	}

	Math::CircleF Collider::GetCircle() const noexcept
	{
	    return std::get<Math::CircleF>(_shape);
	}

	Math::RectangleF Collider::GetRectangle() const noexcept
	{
		return std::get<Math::RectangleF>(_shape);
	}

	Math::PolygonF Collider::GetPolygon() const noexcept
	{
        return std::get<Math::PolygonF>(_shape);
	}

	Math::ShapeType Collider::GetShapeType() const noexcept
	{
		return _shapeType;
	}
}