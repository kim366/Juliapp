#pragma once

#include "Array.hpp"
#include "Boxing.hpp"
#include "Concepts.hpp"
#include "Helpers.hpp"
#include "Value.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <julia.h>
#include <sstream>
#include <string>

namespace jl
{

inline value eval(const char* src_str_)
{
    jl_value_t* res{jl_eval_string(src_str_)};
    impl::check_err();
    return res;
}

inline value exec_from_file(const char* file_name_)
{
    std::ifstream file{file_name_};
    if (!file.is_open())
        throw load_error{std::string{"Could not load script "} + file_name_};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return eval(buffer.str().c_str());
}

template<typename... ArgTs>
value call(const char* fn_name_, ArgTs&&... args_)
{
    constexpr std::size_t num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);

    impl::make_arg_vec<ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* func{jl_eval_string(fn_name_)};
    impl::check_err();
    jl_value_t* res{jl_call(func, boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
    return res;
}

inline void init()
{
    jl_init();
}

inline void quit(int code_ = 0)
{
    jl_atexit_hook(code_);
}

inline void raise_error(const char* content_)
{
    jl_error(content_);
}

template<typename... ArgTs>
void raise_error(const char* content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

inline void use(const std::string& module_)
{
    jl_eval_string((std::string{"using "} + module_).c_str());
}

inline void use(const char* module_)
{
    use(std::string{module_});
}

inline value eval(const std::string& src_str_)
{
    return eval(src_str_.c_str());
}

inline value exec_from_file(const std::string& file_name_)
{
    return exec_from_file(file_name_.c_str());
}

template<typename... ArgTs>
value call(const std::string& fn_name_, ArgTs... args_)
{
    return call(fn_name_.c_str(), args_...);
}

template<typename... ArgTs>
void raise_error(const std::string& content_, ArgTs... args_)
{
    raise_error(content_.c_str(), args_...);
}

} // namespace jl
