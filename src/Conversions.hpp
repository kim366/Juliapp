#pragma once

#include <julia.h>

struct Vec2
{
    float x, y;
};

namespace jl
{

jl_value_t* convert(Vec2);

} // namespace jl
