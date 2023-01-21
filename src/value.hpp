#pragma once

#include "utility.hpp"

#include <julia.h>

namespace jl
{

class value;

template<typename T>
T value_cast(const value& val);

class value
{
private:
    jl_value_t* raw_;

public:
    template<typename T>
    value(const T& val);

    explicit value(from_raw_t, jl_value_t* raw);

    value(const value& other);
    value& operator=(const value& other);

    value(value&& other) noexcept;
    value& operator=(value&& other) noexcept;

    // C++ disallows zero-sized arrays, so instead we have this overload
    value operator()() const;

    template<typename... Ts>
    value operator()(Ts&&... args) const;

    ~value();

    jl_value_t* raw() const;

protected:
    value(value&& val, jl_datatype_t* expected_type);
    void downcast_assign(value&& val, jl_datatype_t* expected_type);
};


} // namespace jl

#include "value.inl"
