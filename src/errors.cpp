#include "errors.hpp"

namespace jl
{

std::string impl::datatype_name(jl_value_t* type)
{
    return jl_string_ptr(jl_call1(impl::repr_fn, type));
}

bad_value_cast::bad_value_cast(const char* expected_type, jl_value_t* val)
    : std::logic_error{std::string{"Expected "} + expected_type + " but value contains " + impl::datatype_name(jl_typeof(val))}
{
}

} // namespace jl