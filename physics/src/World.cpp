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

	void World::updateCollisions() noexcept
	{
		std::unordered_set<ColliderPair, ColliderPairHash> newColliderPairs;

		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled() || collider.IsFree()) continue;

			for (auto& otherCollider : _colliders)
			{
				if (!otherCollider.IsEnabled() || collider.IsFree()) continue;
				if (collider.GetColliderRef() == otherCollider.GetColliderRef()) continue;
				if (collider.GetBodyRef() == otherCollider.GetBodyRef()) continue;

				if (collide(collider, otherCollider))
				{
					newColliderPairs.insert({ collider.GetColliderRef(), otherCollider.GetColliderRef() });
				}
			}
		}

		for (auto& colliderPair : newColliderPairs)
		{
			// Trigger enter
			if (_colliderPairs.find(colliderPair) == _colliderPairs.end())
			{
				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

				if (colliderA.GetContactListener() != nullptr && colliderA.IsTrigger())
				{
					colliderA.GetContactListener()->OnTriggerEnter(colliderPair.A, colliderPair.B);
				}

				if (colliderB.GetContactListener() != nullptr && colliderB.IsTrigger())
				{
					colliderB.GetContactListener()->OnTriggerEnter(colliderPair.B, colliderPair.A);
				}
			}
			// Trigger stay
			else
			{
				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

				if (colliderA.GetContactListener() != nullptr && colliderA.IsTrigger())
				{
					colliderA.GetContactListener()->OnTriggerStay(colliderPair.A, colliderPair.B);
				}

				if (colliderB.GetContactListener() != nullptr && colliderB.IsTrigger())
				{
					colliderB.GetContactListener()->OnTriggerStay(colliderPair.B, colliderPair.A);
				}
			}
		}

		for (auto& colliderPair : _colliderPairs)
		{
			// Trigger exit
			if (newColliderPairs.find(colliderPair) == newColliderPairs.end())
			{
				Collider& colliderA = GetCollider(colliderPair.A);
				Collider& colliderB = GetCollider(colliderPair.B);

				if (colliderA.GetContactListener() != nullptr && colliderA.IsTrigger())
				{
					colliderA.GetContactListener()->OnTriggerExit(colliderPair.A, colliderPair.B);
				}

				if (colliderB.GetContactListener() != nullptr && colliderB.IsTrigger())
				{
					colliderB.GetContactListener()->OnTriggerExit(colliderPair.B, colliderPair.A);
				}
			}
		}

		_colliderPairs = newColliderPairs;
	}

	bool World::collide(Physics::Collider& colliderA, Physics::Collider& colliderB) noexcept
	{
		if (colliderA.GetBodyRef() == colliderB.GetBodyRef()) return false;

		const auto& bodyA = GetBody(colliderA.GetBodyRef());
		const auto& bodyB = GetBody(colliderB.GetBodyRef());

		auto circleA = colliderA.GetCircle() + bodyA.Position();
		auto circleB = colliderB.GetCircle() + bodyB.Position();
		auto rectA = colliderA.GetRectangle() + bodyA.Position();
		auto rectB = colliderB.GetRectangle() + bodyB.Position();
		auto polyA = colliderA.GetPolygon() + bodyA.Position();
		auto polyB = colliderB.GetPolygon() + bodyB.Position();

		const auto shapeTypeA = colliderA.GetShapeType();
		const auto shapeTypeB = colliderB.GetShapeType();

		if (shapeTypeA == Math::ShapeType::Circle && shapeTypeB == Math::ShapeType::Circle)
		{
			return Math::Intersect(circleA, circleB);
		}
		else if (shapeTypeA == Math::ShapeType::Circle && shapeTypeB == Math::ShapeType::Rectangle)
		{
			return Math::Intersect(circleA, rectB);
		}
		else if (shapeTypeA == Math::ShapeType::Circle && shapeTypeB == Math::ShapeType::Polygon)
		{
			return Math::Intersect(circleA, polyB);
		}
		else if (shapeTypeA == Math::ShapeType::Rectangle && shapeTypeB == Math::ShapeType::Circle)
		{
			return Math::Intersect(rectA, circleB);
		}
		else if (shapeTypeA == Math::ShapeType::Rectangle && shapeTypeB == Math::ShapeType::Rectangle)
		{
			return Math::Intersect(rectA, rectB);
		}
		else if (shapeTypeA == Math::ShapeType::Rectangle && shapeTypeB == Math::ShapeType::Polygon)
		{
			return Math::Intersect(rectA, polyB);
		}
		else if (shapeTypeA == Math::ShapeType::Polygon && shapeTypeB == Math::ShapeType::Circle)
		{
			return Math::Intersect(polyA, circleB);
		}
		else if (shapeTypeA == Math::ShapeType::Polygon && shapeTypeB == Math::ShapeType::Rectangle)
		{
			return Math::Intersect(polyA, rectB);
		}
		else if (shapeTypeA == Math::ShapeType::Polygon && shapeTypeB == Math::ShapeType::Polygon)
		{
			return Math::Intersect(polyA, polyB);
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

		updateCollisions();
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

		for (auto& collider : _colliders)
		{
			if (collider.IsFree()) continue;

			if (collider.GetBodyRef() == bodyRef)
			{
				DestroyCollider(collider.GetColliderRef());
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
			_colliders[i].SetColliderRef({ i, _colliderGenerations[i] });
			_colliders[i].Enable();

			return { i, _colliderGenerations[i] };
		}

		// No free colliders found, create a new one, and increase the size of the vector
		const std::size_t oldSize = _colliders.size();
		_colliders.resize(_colliders.size() * 2);
		_colliderGenerations.resize(_colliderGenerations.size() * 2);

		_colliders[oldSize].SetBodyRef(bodyRef);
		_colliders[oldSize].SetColliderRef({ oldSize, _colliderGenerations[oldSize] });
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
}