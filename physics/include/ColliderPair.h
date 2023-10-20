#pragma once

#include "Ref.h"

namespace Physics
{
	struct ColliderPair
	{
		// A and B can be in any order
		ColliderRef A;
		ColliderRef B;

		bool operator<(const ColliderPair& other) const noexcept
		{
			return A < other.A || (A == other.A && B < other.B);
		}

		bool operator==(const ColliderPair& other) const noexcept
		{
			return A == other.A && B == other.B || A == other.B && B == other.A;
		}

		bool operator!=(const ColliderPair& other) const noexcept
		{
			return !(*this == other);
		}

		bool operator>(const ColliderPair& other) const noexcept
		{
			return !(*this < other) && *this != other;
		}

		bool operator<=(const ColliderPair& other) const noexcept
		{
			return *this < other || *this == other;
		}

		bool operator>=(const ColliderPair& other) const noexcept
		{
			return *this > other || *this == other;
		}
	};

	struct ColliderPairHash
	{
		std::size_t operator()(const ColliderPair& pair) const noexcept
		{
			return pair.A.Index + pair.B.Index + pair.A.Generation + pair.B.Generation;
		}
	};
}