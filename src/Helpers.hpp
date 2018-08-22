#pragma once

#include "Errors.hpp"

#include <julia/julia.h>

namespace jl
{

template<typename ElemT>
class array;

namespace impl
{

void check_err()
{
    if (jl_exception_occurred())
        throw language_error{jl_typeof_str(jl_exception_occurred())};
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
        assert(false &&
               "jl - unsupported array type. "
               "Use boolean, floating point or integral types.");
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
