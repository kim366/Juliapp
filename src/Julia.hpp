#pragma once

#include "Boxing.hpp"
#include "Function.hpp"
#include "Helpers.hpp"
#include "Init.hpp"
#include "Literals.hpp"
#include "StringView.hpp"
#include "Value.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <julia.h>
#include <sstream>
#include <string>

namespace jl
{

template<typename ValT, typename... ArgTs>
value<ValT> make_value(ArgTs&&... args_)
{
    jl_datatype_t* found{impl::find_synced_jl_type<ValT>()};
    jlpp_assert(found && "Requested type not synced");
    jl_value_t* val{jl_new_struct_uninit(found)};
    *reinterpret_cast<ValT*>(jl_data_ptr(val)) =
        ValT(std::forward<ArgTs>(args_)...);
    return val;
}

inline runtime_value eval(util::string_view src_str_)
{
    jl_value_t* res{jl_eval_string(src_str_)};
    impl::check_err();
    return res;
}

inline runtime_value exec_from_file(util::string_view file_name_)
{
    std::ifstream file{file_name_};
    if (!file.is_open())
        throw load_error{std::string{"Could not load script "}
                         + static_cast<const char*>(file_name_)};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return eval(buffer.str().c_str());
}

template<typename... ArgTs>
runtime_value call(function fn_, ArgTs&&... args_)
{
    if (fn_ == nullptr)
        throw language_error{"MethodError"};
    constexpr int num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);
    impl::make_arg_vec<jl_value_t*, ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* res{jl_call(fn_, boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
    return res;
}

template<typename... ArgTs>
runtime_value call(util::string_view fn_name_, ArgTs&&... args_)
{
    return call(function{fn_name_}, std::forward<ArgTs>(args_)...);
}

template<typename... ArgTs>
runtime_value function::operator()(ArgTs&&... args_)
{
    return call(*this, std::forward<ArgTs>(args_)...);
}

inline void raise_error(util::string_view content_)
{
    jl_error(content_);
}

template<typename... ArgTs>
void raise_error(util::string_view content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

inline void init()
{
#ifndef JLPP_MANUAL_INIT
    jlpp_assert(false && "Define JLPP_MANUAL_INIT before using init/quit");
#endif
    impl::init();
}

inline void quit(int status_ = 0)
{
#ifndef JLPP_MANUAL_INIT
    jlpp_assert(false && "Define JLPP_MANUAL_INIT before using init/quit");
#endif
    impl::quit(status_);
}

// inline void use(string_view module_)
//{
//    jl_eval_string((std::string{"using "} + module_).c_str());
//}

} // namespace jl
