#pragma once

#include <cmath>
#include <random>

#include "Core/Types.h"

namespace Wraith
{
    constexpr f32 PI = 3.14159265358979323846f;

    inline f32 Radians(f32 angle) { return angle * PI / 180.0f; }

    inline f32 Degrees(f32 angle) { return angle * 180.0f / PI; }

    template <typename T, typename U>
    inline T Max(const T& x, const U& y)
    {
        return (T)((x < y) ? y : x);
    }

    template <typename T, typename U>
    inline T Min(const T& x, const U& y)
    {
        return (T)(x < y) ? x : y;
    }

    template <typename T, typename U, typename R>
    inline T Clamp(const T& x, const U& min, const R& max)
    {
        return (T)Wraith::Max(min, Wraith::Min(x, max));
    }

    template <typename T, typename T2>
    T Lerp(const T& min, const T& max, const T2& current)
    {
        return min + (max - min) * current;
    }

    template <typename T>
    inline T RandomRange(T min, T max)
    {
        if (min == Max(min, max))
            std::swap(min, max);
        static std::random_device rd;
        static std::mt19937 gen(rd());
        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(gen);
        }
        else
        {
            std::uniform_real_distribution<double> dist(static_cast<double>(min), static_cast<double>(max));
            return static_cast<T>(dist(gen));
        }
    }

    inline f32 Rand() { return RandomRange(0.0f, 1.0f); }
    inline f32 Rand11() { return RandomRange(-1.0f, 1.0f); }
}  // namespace Wraith

// Variant helper
template <class... Ts>
struct variant_visitor : Ts...
{
    using Ts::operator()...;
};

namespace std
{
    template <typename T1, typename T2>
    struct hash<std::pair<T1, T2>>
    {
        std::size_t operator()(const std::pair<T1, T2>& pair) const
        {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };
}  // namespace std
