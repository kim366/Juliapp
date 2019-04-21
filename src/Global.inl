#pragma once

#include <Global.hpp>
#include <Module.hpp>
#include <Symbol.hpp>
#include <Value.hpp>
#include <julia.h>

namespace jl
{

inline global::global(jl_binding_t* binding_) : _binding{binding_} {}
inline global::global(symbol symbol_) : global{main[symbol_]} {}

inline jl_binding_t* global::c_binding()
{
    return _binding;
}

template<typename T,
         typename NoRefT,
         std::enable_if_t<!impl::is_value<NoRefT>{}
                          && !std::is_same_v<NoRefT, generic_value>>*>
global& global::operator=(T&& data_)
{
    return *this = ::jl::value<T>{std::forward<T>(data_)};
}

inline global& global::operator=(const generic_value& value_)
{
    jl_checked_assignment(_binding, value_.c_val());
    return *this;
}

} // namespace jl