#pragma once

#include "Boxing.hpp"
#include "Function.hpp"
#include "GenericString.hpp"
#include "GlobalInstance.hpp"
#include "Helpers.hpp"
#include "Literals.hpp"
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
typed_value<ValT> make_value(ArgTs&&... args_)
{
    jl_datatype_t* found{impl::find_synced_jl_type<ValT>()};
    assert(found && "Requested type not synced");
    jl_value_t* val{jl_new_struct_uninit(found)};
    *reinterpret_cast<ValT*>(jl_data_ptr(val)) =
        ValT(std::forward<ArgTs>(args_)...);
    return val;
}

inline value eval(generic_string src_str_)
{
    jl_value_t* res{jl_eval_string(src_str_)};
    impl::check_err();
    return res;
}

inline value exec_from_file(generic_string file_name_)
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
value call(function fn_, ArgTs&&... args_)
{
    constexpr std::size_t num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);
    impl::make_arg_vec<jl_value_t*, ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* res{jl_call(fn_, boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
    return res;
}

template<typename... ArgTs>
value call(generic_string fn_name_, ArgTs&&... args_)
{
    return call(function{fn_name_}, std::forward<ArgTs>(args_)...);
}

template<typename... ArgTs>
value function::operator()(ArgTs&&... args_)
{
    return call(*this, std::forward<ArgTs>(args_)...);
}

inline void raise_error(generic_string content_)
{
    jl_error(content_);
}

template<typename... ArgTs>
void raise_error(generic_string content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

// inline void use(generic_string module_)
//{
//    jl_eval_string((std::string{"using "} + module_).c_str());
//}

} // namespace jl
