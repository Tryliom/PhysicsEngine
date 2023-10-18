#pragma once

#include "Body.h"

#include <cstdlib>

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

namespace Physics::World
{
	void Init(std::size_t defaultBodySize = 500) noexcept;
	void Update(float deltaTime) noexcept;

	BodyRef CreateBody() noexcept;
	void DestroyBody(BodyRef bodyRef) noexcept;
	Body& GetBody(BodyRef bodyRef);
};