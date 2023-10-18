#include "World.h"
#include "Exception.h"

#include <vector>

namespace Physics::World
{
	static std::vector<Body> _bodies;
    static std::vector<std::size_t> _generations;
    static std::size_t _increaseBodySize = 500;

    //TODO: Change defaultBodySize to be a power of 2 or 1.5

	void Init(std::size_t defaultBodySize) noexcept
	{
		if (defaultBodySize == 0)
		{
			defaultBodySize = 1;
		}

		_increaseBodySize = defaultBodySize;
		_bodies.resize(defaultBodySize);
		_generations.resize(defaultBodySize, 0);
	}

	void Update(float deltaTime) noexcept
	{
		for (auto& body : _bodies)
		{
			if (!body.IsEnabled()) continue;

            //TODO: Remove acceleration from body, and calculate it in here

			body.SetAcceleration(body.Force() / body.Mass());
			body.SetVelocity(body.Velocity() + body.Acceleration() * deltaTime);
			body.SetPosition(body.Position() + body.Velocity() * deltaTime);
			body.SetForce(Math::Vec2F(0, 0));
		}
	}

	BodyRef CreateBody() noexcept
	{
		for (size_t i = 0; i < _bodies.size(); i++)
		{
			if (_bodies[i].IsEnabled()) continue;

			_bodies[i].Enable();

			return { i, _generations[i] };
		}

		// No free bodies found, create a new one, and increase the size of the vector
		_bodies.resize(_bodies.size() + _increaseBodySize);
        _generations.resize(_generations.size() + _increaseBodySize);

		std::size_t index = _bodies.size() - _increaseBodySize;

		_bodies[index].Enable();

		return { index, _generations[index] };
	}

	void DestroyBody(BodyRef bodyRef) noexcept
	{
		_bodies[bodyRef.Index].Disable();
		_generations[bodyRef.Index]++;
	}

	Body& GetBody(BodyRef bodyRef)
	{
		if (_generations[bodyRef.Index] != bodyRef.Generation)
		{
			throw InvalidBodyRefException();
		}

		return _bodies[bodyRef.Index];
	}
}