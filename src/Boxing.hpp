#pragma once

#include "Errors.hpp"

#include <julia/julia.h>
namespace jl
{

class array;

namespace impl
{

template<typename RetT>
RetT unbox(jl_value_t* arg_)
{
    if constexpr (std::is_floating_point_v<RetT>)
    {
        if (jl_typeis(arg_, jl_float32_type))
            return jl_unbox_float32(arg_);
        else if (jl_typeis(arg_, jl_float64_type))
            return jl_unbox_float64(arg_);

        throw value_error{
            "jl - Incorrect result type. Floating-point type requested"};
    }
    else if constexpr (std::is_integral_v<RetT>)
    {
        if constexpr (std::is_signed_v<RetT>)
        {
            if (jl_typeis(arg_, jl_int8_type))
                return jl_unbox_int8(arg_);
            else if (jl_typeis(arg_, jl_int16_type))
                return jl_unbox_int16(arg_);
            else if (jl_typeis(arg_, jl_int32_type))
                return jl_unbox_int32(arg_);
            else if (jl_typeis(arg_, jl_int64_type))
                return jl_unbox_int64(arg_);
        }
        else
        {
            if (jl_typeis(arg_, jl_uint8_type))
                return jl_unbox_uint8(arg_);
            else if (jl_typeis(arg_, jl_uint16_type))
                return jl_unbox_uint16(arg_);
            else if (jl_typeis(arg_, jl_uint32_type))
                return jl_unbox_uint32(arg_);
            else if (jl_typeis(arg_, jl_uint64_type))
                return jl_unbox_uint64(arg_);
        }

        throw value_error{
            "jl - Incorrect result type. Integral type requested"};
    }
    else if constexpr (std::is_same_v<RetT, bool>)
    {
        if (jl_typeis(arg_, jl_bool_type))
            return jl_unbox_bool(arg_);

        throw value_error{"jl - Incorrect result type. Boolean type requested"};
    }
    else
    {
        assert(false &&
               "jl - unsupported result type. "
               "Use boolean, floating point or integral types.");
    }

} // namespace impl

template<typename ArgT>
jl_value_t* box(ArgT arg_)
{
    if constexpr (std::is_same<ArgT, bool>())
        return jl_box_bool(arg_);
    else if constexpr (std::is_same<ArgT, std::int8_t>())
        return jl_box_int8(arg_);
    else if constexpr (std::is_same<ArgT, std::uint8_t>())
        return jl_box_uint8(arg_);
    else if constexpr (std::is_same<ArgT, std::int16_t>())
        return jl_box_int16(arg_);
    else if constexpr (std::is_same<ArgT, std::uint16_t>())
        return jl_box_uint16(arg_);
    else if constexpr (std::is_same<ArgT, std::int32_t>())
        return jl_box_int32(arg_);
    else if constexpr (std::is_same<ArgT, std::uint32_t>())
        return jl_box_uint32(arg_);
    else if constexpr (std::is_same<ArgT, std::int64_t>())
        return jl_box_int64(arg_);
    else if constexpr (std::is_same<ArgT, std::uint64_t>())
        return jl_box_uint64(arg_);
    else if constexpr (std::is_same<ArgT, float>())
        return jl_box_float32(arg_);
    else if constexpr (std::is_same<ArgT, double>())
        return jl_box_float64(arg_);
    else if constexpr (std::is_same<ArgT, void*>())
        return jl_box_voidpointer(arg_);
    else if constexpr (std::is_same<ArgT, array>())
        return reinterpret_cast<jl_value_t*>(arg_.data());
    else
    {
        assert(false &&
               "jl - unsupported argument type. "
               "Use array, boolean, floating point or integral types.");
    }
}

} // namespace impl

} // namespace jl
