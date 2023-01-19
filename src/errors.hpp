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

struct error : std::runtime_error
{
protected:
    using std::runtime_error::runtime_error;
};

struct language_error : error
{
    using error::error;
};

struct bad_value_cast : error
{
    bad_value_cast(const char* expected_type, jl_value_t* val)
        : error{std::string{"Expected "} + expected_type + " but value contains " + impl::datatype_name(jl_typeof(val))}
    {
    }
};

struct load_error : error
{
    using error::error;
};

} // namespace jl
