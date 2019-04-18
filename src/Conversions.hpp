#pragma once

#include <Binding.hpp>
#include <Function.hpp>
#include <Module.hpp>
#include <Value.hpp>

namespace jl
{

inline function generic_value::as_function()
{
    return c_val();
}

inline module generic_value::as_module()
{
    return reinterpret_cast<jl_module_t*>(c_val());
}

inline generic_value function::genric()
{
    return c_fn();
}

inline generic_value module::genric()
{
    return reinterpret_cast<jl_value_t*>(c_mod());
}

} // namespace jl
