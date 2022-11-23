#include <utility>

#pragma once

#include "value.hpp"

#include <julia/julia.h>
#include <string>

namespace jl
{

struct function : public value
{
public:
    function(const value& fn);
    function(value&& fn);

    using value::operator=;

    value operator()() const;

    template<typename T, typename... Ts>
    value operator()(T&& first, Ts&&... args) const;

    jl_function_t* raw() const;
    static function from_raw(jl_function_t* raw);
};

function::function(const value& fn)
    : value{fn}
{
}

function::function(value&& fn)
    : value{std::move(fn)}
{
}

value function::operator()() const
{
    return value::from_raw(jl_call0(raw()));
}

template<typename T, typename... Ts>
value function::operator()(T&& first, Ts&&... args) const
{
    value values[]{std::forward<T>(first), std::forward<Ts>(args)...};
    constexpr int num_args{sizeof...(args) + 1};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);

    for (auto i = 0; i < num_args; ++i)
        boxed_args[i] = values[i].raw();

    auto result = value::from_raw(jl_call(raw(), boxed_args, num_args));

    JL_GC_POP();
    return result;
}

function function::from_raw(jl_function_t* raw)
{
    return value::from_raw(reinterpret_cast<jl_value_t*>(raw));
}

jl_function_t* function::raw() const
{
    return static_cast<jl_function_t*>(value::raw());
}

} // namespace jl
