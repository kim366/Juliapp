#pragma once

#include <JuliaBoxing.hpp>
#include <array>
#include <fstream>
#include <julia/julia.h>
#include <sstream>
#include <string>

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
        if constexpr (std::is_integral_v<TargT>)
            return static_cast<TargT>(impl::unbox<long>(_boxed_value));
        else if constexpr (std::is_floating_point_v<TargT>)
            return static_cast<TargT>(impl::unbox<double>(_boxed_value));
        else
            return impl::unbox<TargT>(_boxed_value);
    }

    template<typename TargT>
    operator TargT()
    {
        return impl::unbox<TargT>(_boxed_value);
    }

private:
    jl_value_t* _boxed_value;
};

value exec(const char* src_str_)
{
    return jl_eval_string(src_str_);
}

value exec_from_file(const char* file_name_)
{
    std::ifstream file{file_name_};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return exec(buffer.str().c_str());
}

template<typename... ArgTs>
value call(const char* fn_name_, ArgTs... args_)
{
    std::array<jl_value_t*, sizeof...(args_)> boxed_args{impl::box(args_)...};

    jl_value_t* func = jl_eval_string(fn_name_);
    return jl_call(func, boxed_args.data(), boxed_args.size());
}

void init()
{
    jl_init();
}
void quit(int code_ = 0)
{
    jl_atexit_hook(code_);
}

} // namespace jl
