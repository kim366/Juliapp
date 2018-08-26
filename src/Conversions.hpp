#pragma once

#include <conversion.hpp>

struct Vec2
{
    float x, y;
};

namespace jl
{

boxed_value convert(Vec2);

} // namespace jl
