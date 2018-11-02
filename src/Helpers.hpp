#pragma once

#include "Errors.hpp"
#include "Sync.hpp"

#include <julia.h>

namespace jl
{

template<typename ElemT>
class array;

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

template<typename ElemT>
jl_datatype_t* get_type()
{
    if constexpr (std::is_same_v<ElemT, std::int8_t>)
        return jl_int8_type;
    else if constexpr (std::is_same_v<ElemT, std::uint8_t>)
        return jl_uint8_type;
    else if constexpr (std::is_same_v<ElemT, std::int16_t>)
        return jl_int16_type;
    else if constexpr (std::is_same_v<ElemT, std::uint16_t>)
        return jl_uint16_type;
    else if constexpr (std::is_same_v<ElemT, std::int32_t>)
        return jl_int32_type;
    else if constexpr (std::is_same_v<ElemT, std::uint32_t>)
        return jl_uint32_type;
    else if constexpr (std::is_same_v<ElemT, std::int64_t>)
        return jl_int64_type;
    else if constexpr (std::is_same_v<ElemT, std::uint64_t>)
        return jl_uint64_type;
    else if constexpr (std::is_same_v<ElemT, float>)
        return jl_float32_type;
    else if constexpr (std::is_same_v<ElemT, double>)
        return jl_float64_type;
    else if constexpr (std::is_same_v<ElemT, bool>)
        return jl_bool_type;
    else
    {
        auto found{impl::find_synced_jl_type<ElemT>()};
        jlpp_assert(found &&
               "jl - unsupported array type. "
               "Use boolean, floating point or integral types.");
        return found;
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

} // namespace impl

} // namespace jl
