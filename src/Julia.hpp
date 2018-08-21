#pragma once

#include <JuliaBoxing.hpp>
#include <array>
#include <julia/julia.h>

namespace jl
{

class value
{
public:
    value(jl_value_t* boxed_value_) : _boxed_value{boxed_value_} {}

    value() = default;
    value(const value&) = default;
    value(value&&) = default;

    template<typename TargT>
    TargT get()
    {
        return impl::unbox<TargT>(_boxed_value);
    }

    template<typename TargT>
    operator TargT()
    {
        return get<TargT>();
    }

private:
    jl_value_t* _boxed_value;
};

value exec(const char* src_str_)
{
    return jl_eval_string(src_str_);
}

template<typename... ArgTs>
value call(const char* fn_name_, ArgTs... args_)
{
    std::array<jl_value_t*, sizeof...(args_)> boxed_args{impl::box(args_)...};

    jl_value_t* func = jl_eval_string(fn_name_);
    return jl_call(func, boxed_args.data(), boxed_args.size());
}

} // namespace jl
