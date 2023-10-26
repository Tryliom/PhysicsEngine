#pragma once

struct Color
{
    unsigned char R { 0 };
    unsigned char G { 0 };
    unsigned char B { 0 };
    unsigned char A { 255 };

    static constexpr Color Red() noexcept { return Color(255, 0, 0); }
    static constexpr Color Green() noexcept { return Color(0, 255, 0); }
    static constexpr Color Blue() noexcept { return Color(0, 0, 255); }
    static constexpr Color White() noexcept { return Color(255, 255, 255); }
    static constexpr Color Black() noexcept { return Color(0, 0, 0); }
};