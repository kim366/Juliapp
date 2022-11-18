#pragma once

#include "errors.hpp"
#include <julia/julia.h>

namespace jl::impl
{

template<typename T>
T unbox(jl_value_t* val)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        if (jl_typeis(val, jl_float32_type))
            return jl_unbox_float32(val);
        else if (jl_typeis(val, jl_float64_type))
            return jl_unbox_float64(val);

        throw result_type_error{
            "Incorrect result type. Floating-point type requested"};
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        if (jl_typeis(val, jl_bool_type))
            return jl_unbox_bool(val);

        throw result_type_error{
            "Incorrect result type. Boolean type requested"};
    }
    else if constexpr (std::is_integral_v<T>)
    {
        if constexpr (std::is_signed_v<T>)
        {
            if (jl_typeis(val, jl_int8_type))
                return static_cast<T>(jl_unbox_int8(val));
            else if (jl_typeis(val, jl_int16_type))
                return static_cast<T>(jl_unbox_int16(val));
            else if (jl_typeis(val, jl_int32_type))
                return static_cast<T>(jl_unbox_int32(val));
            else if (jl_typeis(val, jl_int64_type))
                return static_cast<T>(jl_unbox_int64(val));
        }
        else
        {
            if (jl_typeis(val, jl_uint8_type))
                return static_cast<T>(jl_unbox_uint8(val));
            else if (jl_typeis(val, jl_uint16_type))
                return static_cast<T>(jl_unbox_uint16(val));
            else if (jl_typeis(val, jl_uint32_type))
                return static_cast<T>(jl_unbox_uint32(val));
            else if (jl_typeis(val, jl_uint64_type))
                return static_cast<T>(jl_unbox_uint64(val));
        }

        throw result_type_error{
            "Incorrect result type. Integral type requested"};
    }
    else
    {
        throw result_type_error{"unsupported type"};
    }
}

template<typename T>
jl_value_t* box(const T& val)
{
    if constexpr (std::is_same<T, bool>())
        return jl_box_bool(val);
    else if constexpr (std::is_integral_v<T>)
    {
        if constexpr (std::is_signed_v<T>)
        {
            if constexpr (sizeof(T) == 1)
                return jl_box_int8(val);
            else if constexpr (sizeof(T) == 2)
                return jl_box_int16(val);
            else if constexpr (sizeof(T) == 4)
                return jl_box_int32(val);
            else if constexpr (sizeof(T) == 8)
                return jl_box_int64(val);
        }
        else
        {
            if constexpr (sizeof(T) == 1)
                return jl_box_uint8(val);
            else if constexpr (sizeof(T) == 2)
                return jl_box_uint16(val);
            else if constexpr (sizeof(T) == 4)
                return jl_box_uint32(val);
            else if constexpr (sizeof(T) == 8)
                return jl_box_uint64(val);
        }
    }
    else if constexpr (std::is_same<T, float>())
        return jl_box_float32(val);
    else if constexpr (std::is_same<T, double>())
        return jl_box_float64(val);
    else if constexpr (std::is_same<T, void*>())
        return jl_box_voidpointer(val);
    else
    {
       throw result_type_error{"unsupported type"};
    }
}

} // namespace jl::impl
