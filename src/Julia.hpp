#pragma once

#include <JuliaBoxing.hpp>
#include <array>
#include <julia/julia.h>

struct JuliaValue
{
    template<typename TargT>
    TargT get()
    {
        return impl::unbox<TargT>(boxed_value);
    }

    template<typename TargT>
    operator TargT()
    {
        return get<TargT>();
    }

    jl_value_t* boxed_value;
};

JuliaValue jl(const char* src_str_)
{
    return {jl_eval_string(src_str_)};
}

template<typename... ArgTs>
JuliaValue jl(const char* fn_name_, ArgTs... args_)
{
    std::array<jl_value_t*, sizeof...(args_)> boxed_args{impl::box(args_)...};

    jl_value_t* func = jl_eval_string(fn_name_);
    return {jl_call(func, boxed_args.data(), boxed_args.size())};
}
