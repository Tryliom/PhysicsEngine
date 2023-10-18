#pragma once

#include "Body.h"

#include <cstdlib>
#include <vector>

namespace Physics
{
	struct BodyRef
	{
        std::size_t Index;
        std::size_t Generation;

		constexpr bool operator==(const BodyRef& other) const noexcept
		{
			return Index == other.Index && Generation == other.Generation;
		}
	};
}

//TODO: Transform into a class
namespace Physics
{
    class World
    {
    public:
        World(std::size_t defaultBodySize = 500) noexcept;

    private:
        std::vector<Body> _bodies;
        std::vector<std::size_t> _generations;

    public:
        void Update(float deltaTime) noexcept;

        BodyRef CreateBody() noexcept;
        void DestroyBody(BodyRef bodyRef);
        Body& GetBody(BodyRef bodyRef);
    };
}