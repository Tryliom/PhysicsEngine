#include "World.h"
#include "Body.h"
#include "Exception.h"

#include <vector>

namespace World
{
	std::vector<Body> _bodies;
	std::vector<size_t> _generations;
	std::size_t _increaseBodySize = 500;

	void Init(size_t defaultBodySize) noexcept
	{
		if (defaultBodySize == 0)
		{
			defaultBodySize = 1;
		}

		_increaseBodySize = defaultBodySize;
		_bodies.reserve(defaultBodySize);
		_generations.reserve(defaultBodySize);

		for (size_t i = 0; i < defaultBodySize; i++)
		{
			_bodies.emplace_back();
			_generations.push_back(0);
		}
	}

	void Update(float deltaTime) noexcept
	{
		for (auto& body : _bodies)
		{
			if (!body.IsEnabled()) continue;

			body.Velocity += body.Acceleration * deltaTime;
			body.Position += body.Velocity * deltaTime;
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
		_bodies.reserve(_bodies.size() + _increaseBodySize);

		for (size_t i = 0; i < _increaseBodySize; i++)
		{
			_bodies.emplace_back();
			_generations.push_back(0);
		}

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