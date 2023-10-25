#pragma once

#include <cstdlib>

namespace Physics
{
    template<int i>
    struct Ref
    {
        std::size_t Index;
        std::size_t Generation;

        constexpr bool operator==(const Ref& other) const noexcept
        {
            return Index == other.Index && Generation == other.Generation;
        }

        constexpr bool operator!=(const Ref& other) const noexcept
        {
            return *this != other;
        }
    };

    using BodyRef = Ref<0>;
    using ColliderRef = Ref<1>;
}