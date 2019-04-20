#include <utility>

#pragma once

#include "Module.hpp"
#include "Symbol.hpp"

#include <julia.h>
#include <string>

namespace jl
{

struct function : public generic_value
{
    function(generic_value val_) : generic_value{std::move(val_)} {}

    function(symbol symbol_)
        : generic_value{jl_get_global(jl_main_module, symbol_.c_sym())}
    {
    }

    jl_function_t* c_fn() const { return c_val(); }

    template<typename... ArgTs>
    generic_value operator()(ArgTs&&... args_);

    bool operator==(const function& rhs) const { return c_fn() == rhs.c_fn(); }

    bool operator!=(const function& rhs) const { return !(rhs == *this); }
};

namespace impl
{

template<typename... ArgTs>
generic_value call(function fn_, ArgTs&&... args_)
{
    if (fn_.c_fn() == nullptr)
        throw language_error{"MethodError"};
    constexpr int num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);
    impl::make_arg_vec<jl_value_t*, ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* res{jl_call(fn_.c_fn(), boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
    return res;
}

} // namespace impl

template<typename... ArgTs>
generic_value function::operator()(ArgTs&&... args_)
{
    return impl::call(*this, std::forward<ArgTs>(args_)...);
}

} // namespace jl
