#pragma once

#include "julia.hpp"

#include <string>

#define UNUSED(x) do { (void)x; } while(0)

template<typename S>
std::string repr(S* subject)
{
    return jl_string_ptr(jl_call1(jl::impl::repr_fn, reinterpret_cast<jl_value_t*>(subject)));
}

template<typename S>
std::string repr(const S& subject)
{
    return repr(subject.raw());
}

inline int num_rooted()
{
    return jl::impl::rooted_values.size();
}

inline int num_rooted(jl_value_t* value)
{
    const auto begin = jl::impl::rooted_values.begin();
    const auto end = jl::impl::rooted_values.end();
    return std::count(begin, end, value);
}
