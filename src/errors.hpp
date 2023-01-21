#pragma once

#include "externs.hpp"

#include <stdexcept>

namespace jl
{

namespace impl
{

std::string datatype_name(jl_value_t* type);

} // namespace impl

struct bad_value_cast : std::logic_error
{
    bad_value_cast(const char* expected_type, jl_value_t* val);
};

} // namespace jl
