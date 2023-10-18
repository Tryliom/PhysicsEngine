#include "World.h"
#include "Exception.h"

#include <vector>

namespace Physics::World
{
	static std::vector<Body> _bodies;
    static std::vector<std::size_t> _generations;

	void Init(std::size_t defaultBodySize) noexcept
	{
		if (defaultBodySize == 0)
		{
			defaultBodySize = 1;
		}

		_bodies.resize(defaultBodySize);
		_generations.resize(defaultBodySize, 0);
	}

	void Update(float deltaTime) noexcept
	{
		for (auto& body : _bodies)
		{
			if (!body.IsEnabled()) continue;

			body.SetVelocity(body.Velocity() + body.Force() * deltaTime);
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
        const std::size_t oldSize = _bodies.size();
		_bodies.resize(_bodies.size() * 2);
        _generations.resize(_generations.size() * 2);

		_bodies[oldSize].Enable();

		return { oldSize, _generations[oldSize] };
	}

	void DestroyBody(BodyRef bodyRef)
	{
        if (_generations[bodyRef.Index] != bodyRef.Generation)
        {
            throw InvalidBodyRefException();
        }

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