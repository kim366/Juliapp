#pragma once

#include "julia.hpp"

#include <string>

template<typename S>
std::string repr(const S& subject)
{
    return jl_string_ptr(jl_call1(jl::impl::repr_fn, reinterpret_cast<jl_value_t*>(subject.raw())));
}

inline int num_rooted()
{
    return jl::impl::rooted_values.size();
}
