#pragma once

#include <cstdlib>

namespace Physics
{
	struct ColliderRef
	{
		std::size_t Index;
		std::size_t Generation;

		constexpr bool operator==(const ColliderRef& other) const noexcept
		{
			return Index == other.Index && Generation == other.Generation;
		}

		constexpr bool operator!=(const ColliderRef& other) const noexcept
		{
			return !(*this == other);
		}

		constexpr bool operator<(const ColliderRef& other) const noexcept
		{
			return Index < other.Index || (Index == other.Index && Generation < other.Generation);
		}

		constexpr bool operator>(const ColliderRef& other) const noexcept
		{
			return Index > other.Index || (Index == other.Index && Generation > other.Generation);
		}

		constexpr bool operator<=(const ColliderRef& other) const noexcept
		{
			return *this < other || *this == other;
		}

		constexpr bool operator>=(const ColliderRef& other) const noexcept
		{
			return *this > other || *this == other;
		}
	};

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