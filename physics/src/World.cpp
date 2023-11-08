#include "World.h"

#include "Exception.h"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace Physics
{
	World::World(HeapAllocator& heapAllocator, std::size_t defaultBodySize) noexcept :
        _heapAllocator(heapAllocator),
        _colliderPairs{ StandardAllocator<ColliderPair> {heapAllocator} },
		_bodies { StandardAllocator<Body> {heapAllocator} },
		_colliders { StandardAllocator<Collider> {heapAllocator} },
		_colliderGenerations { StandardAllocator<std::size_t> {heapAllocator} },
		_bodyGenerations { StandardAllocator<std::size_t> {heapAllocator} }
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
#ifdef TRACY_ENABLE
		ZoneNamedN(updateColliders, "World::updateColliders", true);
#endif
		// Calculate minimum and maximum bounds of all colliders
		float minX = std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float maxY = std::numeric_limits<float>::min();

		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled() || collider.IsFree()) continue;

			const auto& bounds = collider.GetBounds();

			const auto& min = bounds.MinBound();
			const auto& max = bounds.MaxBound();

			if (min.X < minX) minX = min.X;
			if (min.Y < minY) minY = min.Y;
			if (max.X > maxX) maxX = max.X;
			if (max.Y > maxY) maxY = max.Y;
		}

		// Clear all colliders from the quadtree
		_quadTree.ClearColliders();

		// Update the boundary of the quadtree
		_quadTree.UpdateBoundary(Math::RectangleF({ minX, minY }, { maxX, maxY }));

		// Insert all colliders into the quadtree
		insertColliders();

		// Check for collisions and triggers
		processColliders();
	}

	void World::insertColliders() noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(insertColliders, "World::insertColliders", true);
#endif

		// Insert all colliders into the quadtree
		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled() || collider.IsFree()) continue;

			_quadTree.Insert({collider.GetColliderRef(), collider.GetBounds()});
		}
	}

	void World::processColliders() noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(processColliders, "World::processColliders", true);
#endif
        //TODO: Use std::vector of ColliderPair
        std::unordered_set<ColliderPair, ColliderPairHash, std::equal_to<>, StandardAllocator<ColliderPair>> newColliderPairs { StandardAllocator<ColliderPair> {_heapAllocator} };

        //TODO: Get all possible colliders pair at once
		// Check for collisions
		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled() || collider.IsFree()) continue;

			// Get all colliders that overlap with the collider
			const auto& colliders = _quadTree.GetColliders({ collider.GetColliderRef(), collider.GetBounds() });

			for (auto& otherColliderRef : colliders)
			{
				const auto& otherCollider = GetCollider(otherColliderRef);

				if (otherCollider.GetBodyRef() == collider.GetBodyRef()) continue;

				// Check if the colliders overlap
				if (overlap(collider, otherCollider))
				{
					newColliderPairs.insert(ColliderPair{ collider.GetColliderRef(), otherColliderRef });
				}
			}
		}

		for (auto& colliderPair : newColliderPairs)
		{
			Collider& colliderA = GetCollider(colliderPair.A);
			Collider& colliderB = GetCollider(colliderPair.B);

			if (_colliderPairs.find(colliderPair) == _colliderPairs.end())
			{
                if (_contactListener == nullptr) continue;

				// Enter
				if (colliderA.IsTrigger() || colliderB.IsTrigger())
				{
					_contactListener->OnTriggerEnter(colliderPair.A, colliderPair.B);
				}
				else
				{
					_contactListener->OnCollisionEnter(colliderPair.A, colliderPair.B);
				}
			}
			else
			{
				// Stay
				if (colliderA.IsTrigger() || colliderB.IsTrigger())
				{
                    if (_contactListener == nullptr) continue;

					_contactListener->OnTriggerStay(colliderPair.A, colliderPair.B);
				}
				else
				{
                    if (_contactListener != nullptr)
                    {
                        _contactListener->OnCollisionStay(colliderPair.A, colliderPair.B);
                    }

                    const auto& bodyA = GetBody(colliderA.GetBodyRef());
                    const auto& bodyB = GetBody(colliderB.GetBodyRef());

					if (bodyA.GetBodyType() == BodyType::Dynamic || bodyB.GetBodyType() == BodyType::Dynamic)
					{
						onCollision(colliderPair.A, colliderPair.B);
					}
				}
			}
		}

        if (_contactListener != nullptr)
        {
            // Exit
            for (auto& colliderPair: _colliderPairs)
            {
                if (newColliderPairs.find(colliderPair) != newColliderPairs.end()) continue;

                Collider& colliderA = GetCollider(colliderPair.A);
                Collider& colliderB = GetCollider(colliderPair.B);

                if (colliderA.IsTrigger() || colliderB.IsTrigger())
                {
                    _contactListener->OnTriggerExit(colliderPair.A, colliderPair.B);
                }
                else
                {
                    _contactListener->OnCollisionExit(colliderPair.A, colliderPair.B);
                }
            }
        }

		_colliderPairs = newColliderPairs;
	}

	void World::onCollision(Physics::ColliderRef colliderRef, Physics::ColliderRef otherColliderRef) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(onCollision, "World::onCollision", true);
#endif

		auto colliderA = GetCollider(colliderRef);
		auto colliderB = GetCollider(otherColliderRef);

		if (colliderA.GetShapeType() == Math::ShapeType::Polygon || colliderB.GetShapeType() == Math::ShapeType::Polygon) return;
		if (colliderA.GetShapeType() == Math::ShapeType::Circle && colliderB.GetShapeType() == Math::ShapeType::Rectangle) return;
		if (colliderA.GetShapeType() == Math::ShapeType::Rectangle && colliderB.GetShapeType() == Math::ShapeType::Circle) return;

		// 2x circle, overlap already checked
		if (colliderA.GetShapeType() == Math::ShapeType::Circle && colliderB.GetShapeType() == Math::ShapeType::Circle)
		{
			auto& bodyA = GetBody(colliderA.GetBodyRef());
			auto& bodyB = GetBody(colliderB.GetBodyRef());

			const auto& positionA = bodyA.Position() + colliderA.GetOffset();
			const auto& positionB = bodyB.Position() + colliderB.GetOffset();

			const auto& normal = (positionA - positionB).Normalized();
			const auto& relativeVelocity = bodyA.Velocity() - bodyB.Velocity();
			const auto& velocityAlongNormal = relativeVelocity.Dot(normal);

			if (velocityAlongNormal > 0) return;

			const auto& restitution = std::min(colliderA.GetBounciness(), colliderB.GetBounciness());
			const auto& impulseMagnitude = -restitution * velocityAlongNormal;
			const auto& impulse = impulseMagnitude * normal;

            if (bodyA.GetBodyType() == BodyType::Dynamic)
            {
                bodyA.AddVelocity(impulse * bodyA.InverseMass());
            }

            if (bodyB.GetBodyType() == BodyType::Dynamic)
            {
                bodyB.AddVelocity(- impulse * bodyB.InverseMass());
            }
		}
		// 2x rectangle, overlap already checked
		else if (colliderA.GetShapeType() == Math::ShapeType::Rectangle && colliderB.GetShapeType() == Math::ShapeType::Rectangle)
		{
			auto& bodyA = GetBody(colliderA.GetBodyRef());
			auto& bodyB = GetBody(colliderB.GetBodyRef());

			const auto& positionA = bodyA.Position() + colliderA.GetOffset() + colliderA.GetRectangle().Center();
			const auto& positionB = bodyB.Position() + colliderB.GetOffset() + colliderB.GetRectangle().Center();

            //TODO: Check penetration and take the minimum penetration vertical and horizontal

			const auto& normal = (positionA - positionB).Normalized();
			const auto& relativeVelocity = bodyA.Velocity() - bodyB.Velocity();
			const auto& velocityAlongNormal = relativeVelocity.Dot(normal);

			if (velocityAlongNormal > 0) return;

			const auto& restitution = std::min(colliderA.GetBounciness(), colliderB.GetBounciness());
			const auto& impulseMagnitude = -restitution * velocityAlongNormal;
			const auto& impulse = impulseMagnitude * normal;

            if (bodyA.GetBodyType() == BodyType::Dynamic)
            {
                bodyA.AddVelocity(impulse * bodyA.InverseMass());
            }

            if (bodyB.GetBodyType() == BodyType::Dynamic)
            {
                bodyB.AddVelocity(- impulse * bodyB.InverseMass());
            }
		}
	}

	bool World::overlap(const Collider& colliderA, const Collider& colliderB) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(overlap, "World::overlap", true);
#endif

		if (colliderA.GetBodyRef() == colliderB.GetBodyRef()) return false;

        switch (colliderA.GetShapeType())
        {
            case Math::ShapeType::Circle:
            {
                auto circleA = Math::CircleF(colliderA.GetOffset() + colliderA.GetPosition(), colliderA.GetCircle().Radius());

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + colliderB.GetPosition(), colliderB.GetCircle().Radius());

                        return Math::Intersect(circleA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(circleA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(circleA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;

            case Math::ShapeType::Rectangle:
            {
                auto rectA = colliderA.GetRectangle() + colliderA.GetPosition() + colliderA.GetOffset();

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + colliderB.GetPosition(), colliderB.GetCircle().Radius());

                        return Math::Intersect(rectA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(rectA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(rectA, polyB);
                    }
	                case Math::ShapeType::None:break;
                }
            }
            break;

            case Math::ShapeType::Polygon:
            {
                auto polyA = colliderA.GetPolygon() + colliderA.GetPosition();

                switch (colliderB.GetShapeType())
                {
                    case Math::ShapeType::Circle:
                    {
                        auto circleB = Math::CircleF(colliderB.GetOffset() + colliderB.GetPosition(), colliderB.GetCircle().Radius());

                        return Math::Intersect(polyA, circleB);
                    }
                    case Math::ShapeType::Rectangle:
                    {
                        auto rectB = colliderB.GetRectangle() + colliderB.GetPosition() + colliderB.GetOffset();

                        return Math::Intersect(polyA, rectB);
                    }
                    case Math::ShapeType::Polygon:
                    {
                        auto polyB = colliderB.GetPolygon() + colliderB.GetPosition() + colliderB.GetOffset();

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

	void World::updateBodies(float deltaTime) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(updateBodies, "World::updateBodies", true);
#endif
		for (auto& body : _bodies)
		{
			if (!body.IsEnabled() || body.GetBodyType() == BodyType::Static) continue;

            if (body.UseGravity())
            {
                body.AddForce(_gravity * body.InverseMass());
            }

            const auto& acceleration = body.Force() * body.InverseMass() * deltaTime;

			body.AddVelocity(acceleration);
			body.AddPosition(body.Velocity() * deltaTime);
			body.SetForce(Math::Vec2F(0, 0));
		}

		if (_colliders.empty()) return;

		for (auto& collider : _colliders)
		{
			if (!collider.IsEnabled()) continue;

			const auto& body = GetBody(collider.GetBodyRef());

			collider.SetPosition(body.Position() + collider.GetOffset());
		}
	}

    void World::Reset() noexcept
    {
        _colliderPairs.clear();
        _bodies.clear();
        _colliders.clear();
        _colliderGenerations.clear();
        _bodyGenerations.clear();

        _bodies.resize(1);
        _bodyGenerations.resize(1, 0);
        _colliders.resize(1);
        _colliderGenerations.resize(1, 0);
    }

	void World::Update(float deltaTime) noexcept
	{
#ifdef TRACY_ENABLE
		ZoneNamedN(update, "World::Update", true);
#endif
		updateBodies(deltaTime);
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

			const ColliderRef colliderRef = { i, _colliderGenerations[i] };

			_colliders[i].SetBodyRef(bodyRef);
			_colliders[i].Enable();
			_colliders[i].SetColliderRef(colliderRef);

			return colliderRef;
		}

		// No free colliders found, create a new one, and increase the size of the vector
		const std::size_t oldSize = _colliders.size();

		_colliders.resize(_colliders.size() * 2);
		_colliderGenerations.resize(_colliderGenerations.size() * 2);

		const ColliderRef colliderRef = { oldSize, _colliderGenerations[oldSize] };

		_colliders[oldSize].SetBodyRef(bodyRef);
		_colliders[oldSize].Enable();
		_colliders[oldSize].SetColliderRef(colliderRef);

		return colliderRef;
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

	std::vector<Math::RectangleF> World::GetQuadTreeBoundaries() const noexcept
	{
		return _quadTree.GetBoundaries();
	}

    void World::SetGravity(Math::Vec2F gravity) noexcept
    {
        _gravity = gravity;
    }
}