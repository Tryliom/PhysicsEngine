#include "Collider.h"

namespace Physics
{
	ColliderRef Collider::GetColliderRef() const noexcept
	{
		return _colliderRef;
	}

	BodyRef Collider::GetBodyRef() const noexcept
	{
		return _bodyRef;
	}

	ContactListener* Collider::GetContactListener() const noexcept
	{
		return _contactListener;
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
		return _isFree;
	}

	bool Collider::IsEnabled() const noexcept
	{
		return !_isFree && _isEnabled;
	}

	void Collider::SetColliderRef(ColliderRef colliderRef) noexcept
	{
		_colliderRef = colliderRef;
	}

	void Collider::SetBodyRef(BodyRef bodyRef) noexcept
	{
		_bodyRef = bodyRef;
	}

	void Collider::SetContactListener(ContactListener* contactListener) noexcept
	{
		_contactListener = contactListener;
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
		_shape.Circle = circle;
	}

	void Collider::SetRectangle(Math::RectangleF rectangle) noexcept
	{
		_shapeType = Math::ShapeType::Rectangle;
		_shape.Rectangle = rectangle;
	}

	void Collider::SetPolygon(Math::PolygonF polygon) noexcept
	{
		_shapeType = Math::ShapeType::Polygon;
		_shape.Polygon = polygon;
	}

	void Collider::Enable() noexcept
	{
		_isEnabled = true;
		_isFree = false;
	}

	void Collider::Disable() noexcept
	{
		_isEnabled = false;
	}

	void Collider::Free() noexcept
	{
		delete _contactListener;

		_contactListener = nullptr;
		_isEnabled = false;
		_bounciness = 0.f;
		_friction = 0.f;
		_isTrigger = false;
		_isFree = true;
	}

	Math::CircleF Collider::GetCircle() const noexcept
	{
		return _shape.Circle;
	}

	Math::RectangleF Collider::GetRectangle() const noexcept
	{
		return _shape.Rectangle;
	}

	Math::PolygonF Collider::GetPolygon() const noexcept
	{
		return _shape.Polygon;
	}

	Math::ShapeType Collider::GetShapeType() const noexcept
	{
		return _shapeType;
	}
}