#pragma once

#include "Errors.hpp"

#include <julia.h>

namespace jl
{

template<typename ElemT>
class array;

template<typename ValT>
class value;

namespace impl
{

inline void check_err()
{
    jl_value_t* error{jl_exception_occurred()};
    if (error)
    {
#ifdef JULIAPP_DEBUG
        jl_value_t* show_error = jl_get_function(jl_base_module, "showerror");
        jl_call2(show_error, jl_stderr_obj(), error);
        jl_printf(JL_STDERR, "\n");
#endif
        throw language_error{jl_typeof_str(error)};
    }
}

template<typename>
struct is_array : std::false_type
{
};

template<typename ElemT>
struct is_array<array<ElemT>> : std::true_type
{
};

template<typename>
struct is_value : std::false_type
{
};

template<typename ValT>
struct is_value<value<ValT>> : std::true_type
{
};

} // namespace impl

} // namespace jl
