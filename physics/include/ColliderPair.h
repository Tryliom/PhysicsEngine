#pragma once

#include "Ref.h"

namespace Physics
{
    /**
     * @brief A pair of colliders
     */
	struct ColliderPair
	{
		// A and B can be in any order
		ColliderRef A;
		ColliderRef B;

		bool operator==(const ColliderPair& other) const noexcept
		{
			return A == other.A && B == other.B || A == other.B && B == other.A;
		}
	};

    /**
     * @brief Hash function for ColliderPair
     */
	struct ColliderPairHash
	{
		std::size_t operator()(const ColliderPair& pair) const noexcept
		{
			return pair.A.Index + pair.B.Index + pair.A.Generation + pair.B.Generation;
		}
	};
}