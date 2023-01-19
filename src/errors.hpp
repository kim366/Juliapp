#pragma once

#include "externs.hpp"

#include <stdexcept>

namespace jl
{

namespace impl
{

inline std::string datatype_name(jl_value_t* type)
{
    return jl_string_ptr(jl_call1(impl::repr_fn, type));
}

} // namespace impl

struct bad_value_cast : std::logic_error
{
    bad_value_cast(const char* expected_type, jl_value_t* val)
        : std::logic_error{std::string{"Expected "} + expected_type + " but value contains " + impl::datatype_name(jl_typeof(val))}
    {
    }
};

} // namespace jl
