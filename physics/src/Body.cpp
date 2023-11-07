#include "Body.h"

namespace Physics
{
	Body::Body(Math::Vec2F position, Math::Vec2F velocity) noexcept
	{
		_position = position;
		_velocity = velocity;
	}

	[[nodiscard]] Math::Vec2F Body::Position() const noexcept
	{
		return _position;
	}

	void Body::SetPosition(Math::Vec2F position) noexcept
	{
		_position = position;
	}

	[[nodiscard]] Math::Vec2F Body::Velocity() const noexcept
	{
		return _velocity;
	}

	void Body::SetVelocity(Math::Vec2F velocity) noexcept
	{
		_velocity = velocity;
	}

	[[nodiscard]] Math::Vec2F Body::Force() const noexcept
	{
		return _force;
	}

	void Body::SetForce(Math::Vec2F force) noexcept
	{
		_force = force;
	}

	[[nodiscard]] float Body::Mass() const noexcept
	{
		return _mass;
	}

	[[nodiscard]] float Body::InverseMass() const noexcept
	{
		return _inverseMass;
	}

	void Body::SetMass(float mass) noexcept
	{
		if (mass <= 0.f) return;

		_mass = mass;
		_inverseMass = 1.f / mass;
	}

	void Body::ApplyForce(Math::Vec2F force) noexcept
	{
		_force += force;
	}

	void Body::Disable() noexcept
	{
		_mass = -1.f;

		_position = Math::Vec2F(0, 0);
		_velocity = Math::Vec2F(0, 0);
		_force = Math::Vec2F(0, 0);
		_inverseMass = 0.f;
	}

	void Body::Enable() noexcept
	{
		_mass = 1.f;
		_inverseMass = 1.f;
	}

	[[nodiscard]] bool Body::IsEnabled() const noexcept
	{
		return _mass > 0.f;
	}
}