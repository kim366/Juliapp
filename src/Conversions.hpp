#pragma once

#include <Function.hpp>
#include <Global.hpp>
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

inline generic_value global::value()
{
    assert(c_binding()->value != nullptr);
    return c_binding()->value;
}

inline function global::as_function()
{
    return value();
}

inline module global::as_module()
{
    return value();
}

template<typename TargT>
global::operator TargT()
{
    return value().get<TargT>();
}

} // namespace jl
