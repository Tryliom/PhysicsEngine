#include "World.h"

#include "Exception.h"

namespace Physics
{
	World::World(std::size_t defaultBodySize) noexcept
	{
		if (defaultBodySize == 0)
		{
			defaultBodySize = 1;
		}

		_bodies.resize(defaultBodySize);
		_bodyGenerations.resize(defaultBodySize, 0);
		_colliders.resize(defaultBodySize);
		_colliderGenerations.resize(defaultBodySize, 0);
	}

	void World::updateColliders() noexcept
	{
		std::unordered_set<ColliderPair, ColliderPairHash> newColliderPairs;

        if (_contactListener == nullptr) return;

		for (std::size_t i = 0; i < _colliders.size(); i++)
		{
            auto& collider = _colliders[i];

			if (!collider.IsEnabled() || collider.IsFree()) continue;

			for (std::size_t j = i + 1; j < _colliders.size(); j++)
			{
                auto& otherCollider = _colliders[j];

				if (!otherCollider.IsEnabled() || collider.IsFree()) continue;

                auto colliderRefA = ColliderRef{ i, _colliderGenerations[i] };
                auto colliderRefB = ColliderRef{ j, _colliderGenerations[j] };

				if (colliderRefA == colliderRefB) continue;
				if (collider.GetBodyRef() == otherCollider.GetBodyRef()) continue;

				if (overlap(collider, otherCollider))
				{
					newColliderPairs.insert({ colliderRefA, colliderRefB });
				}
			}
		}

		for (auto& colliderPair : newColliderPairs)
		{
			// TriggerInfo enter
			if (_colliderPairs.find(colliderPair) == _colliderPairs.end())
			{
				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

				if (colliderA.IsTrigger() || colliderB.IsTrigger())
				{
					_contactListener->OnTriggerEnter(colliderPair.A, colliderPair.B);
				}
			}
			// TriggerInfo stay
			else
			{
				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

                if (colliderA.IsTrigger() || colliderB.IsTrigger())
                {
                    _contactListener->OnTriggerStay(colliderPair.A, colliderPair.B);
				}
			}
		}

		for (auto& colliderPair : _colliderPairs)
		{
			// TriggerInfo exit
			if (newColliderPairs.find(colliderPair) == newColliderPairs.end())
			{
				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

                if (colliderA.IsTrigger() || colliderB.IsTrigger())
                {
                    _contactListener->OnTriggerExit(colliderPair.A, colliderPair.B);
				}
			}
		}

		_colliderPairs = newColliderPairs;
	}

	bool World::overlap(Collider& colliderA, Collider& colliderB) noexcept
	{
		if (colliderA.GetBodyRef() == colliderB.GetBodyRef()) return false;

		const auto& bodyA = GetBody(colliderA.GetBodyRef());
		const auto& bodyB = GetBody(colliderB.GetBodyRef());

        const auto positionA = bodyA.Position() + colliderA.GetOffset();
        const auto positionB = bodyB.Position() + colliderB.GetOffset();

        switch (colliderA.GetShapeType())
        {
            case Math::ShapeType::Circle:
            {
                auto circleA = Math::CircleF(colliderA.GetOffset() + positionA, colliderA.GetCircle().Radius());

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + positionB, colliderB.GetCircle().Radius());

                        return Math::Intersect(circleA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + positionB + colliderB.GetOffset();

                        return Math::Intersect(circleA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + positionB + colliderB.GetOffset();

                        return Math::Intersect(circleA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                auto rectA = colliderA.GetRectangle() + positionA + colliderA.GetOffset();

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + positionB, colliderB.GetCircle().Radius());

                        return Math::Intersect(rectA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + positionB + colliderB.GetOffset();

                        return Math::Intersect(rectA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + positionB + colliderB.GetOffset();

                        return Math::Intersect(rectA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;

            case Math::ShapeType::Polygon:
            {
                auto polyA = colliderA.GetPolygon() + positionA;

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + positionB, colliderB.GetCircle().Radius());

                        return Math::Intersect(polyA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + positionB + colliderB.GetOffset();

                        return Math::Intersect(polyA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + positionB + colliderB.GetOffset();

                        return Math::Intersect(polyA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;
            case Math::ShapeType::None: break;
        }

		return false;
	}

	void World::Update(float deltaTime) noexcept
	{
		for (auto& body : _bodies)
		{
			if (!body.IsEnabled()) continue;

			body.SetVelocity(body.Velocity() + body.Force() * deltaTime);
			body.SetPosition(body.Position() + body.Velocity() * deltaTime);
			body.SetForce(Math::Vec2F(0, 0));
		}

        updateColliders();
	}

	BodyRef World::CreateBody() noexcept
	{
		for (size_t i = 0; i < _bodies.size(); i++)
		{
			if (_bodies[i].IsEnabled()) continue;

			_bodies[i].Enable();

			return {i, _bodyGenerations[i] };
		}

		// No free bodies found, create a new one, and increase the size of the vector
        const std::size_t oldSize = _bodies.size();
		_bodies.resize(_bodies.size() * 2);
        _bodyGenerations.resize(_bodyGenerations.size() * 2);

		_bodies[oldSize].Enable();

		return {oldSize, _bodyGenerations[oldSize] };
	}

	void World::DestroyBody(BodyRef bodyRef)
	{
        if (_bodyGenerations[bodyRef.Index] != bodyRef.Generation)
        {
            throw InvalidBodyRefException();
        }

		for (std::size_t i = 0; i < _colliders.size(); i++)
		{
            auto& collider = _colliders[i];

			if (collider.IsFree()) continue;

			if (collider.GetBodyRef() == bodyRef)
			{
				DestroyCollider({ i, _colliderGenerations[i] });
            }
		}

		_bodies[bodyRef.Index].Disable();
		_bodyGenerations[bodyRef.Index]++;
	}

	Body& World::GetBody(BodyRef bodyRef)
	{
		if (_bodyGenerations[bodyRef.Index] != bodyRef.Generation)
		{
			throw InvalidBodyRefException();
		}

		return _bodies[bodyRef.Index];
	}

	ColliderRef World::CreateCollider(BodyRef bodyRef) noexcept
	{
		for (size_t i = 0; i < _colliders.size(); i++)
		{
			if (!_colliders[i].IsFree()) continue;

			_colliders[i].SetBodyRef(bodyRef);
			_colliders[i].Enable();

			return { i, _colliderGenerations[i] };
		}

		// No free colliders found, create a new one, and increase the size of the vector
		const std::size_t oldSize = _colliders.size();
		_colliders.resize(_colliders.size() * 2);
		_colliderGenerations.resize(_colliderGenerations.size() * 2);

		_colliders[oldSize].SetBodyRef(bodyRef);
		_colliders[oldSize].Enable();

		return { oldSize, _colliderGenerations[oldSize] };
	}

	void World::DestroyCollider(ColliderRef colliderRef)
	{
		_colliders[colliderRef.Index].Free();
		_colliderGenerations[colliderRef.Index]++;
	}

	Collider& World::GetCollider(ColliderRef colliderRef)
	{
		if (_colliderGenerations[colliderRef.Index] != colliderRef.Generation)
		{
			throw InvalidColliderRefException();
		}

		return _colliders[colliderRef.Index];
	}

    void World::SetContactListener(ContactListener* contactListener) noexcept
    {
        _contactListener = contactListener;
    }
}