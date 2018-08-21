#pragma once

#include <JuliaBoxing.hpp>
#include <array>
#include <fstream>
#include <julia/julia.h>
#include <sstream>
#include <string>

namespace jl
{

struct error : public std::runtime_error
{
    explicit error(const std::string& str_) : std::runtime_error{str_} {}
    explicit error(const char* str_) : std::runtime_error{str_} {}
    virtual ~error() {}
};

namespace impl
{
void check_err()
{
    if (jl_exception_occurred())
        throw error{jl_typeof_str(jl_exception_occurred())};
}
} // namespace impl

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
    jl_value_t* res{jl_eval_string(src_str_)};
    impl::check_err();
    return res;
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

    jl_value_t* func{jl_eval_string(fn_name_)};
    impl::check_err();
    jl_value_t* res{jl_call(func, boxed_args.data(), boxed_args.size())};
    impl::check_err();
    return res;
}

void init()
{
    jl_init();
}

void quit(int code_ = 0)
{
    jl_atexit_hook(code_);
}

void raise_error(const char* content_)
{
    jl_error(content_);
}

template<typename... ArgTs>
void raise_error(const char* content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

} // namespace jl
