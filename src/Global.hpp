#pragma once

#include <Module.hpp>
#include <Symbol.hpp>
#include <Value.hpp>
#include <julia.h>

namespace jl
{

class generic_value;
class function;
class module;

class global
{
public:
    global(jl_binding_t* binding_) : _binding{binding_} {}
    global(symbol symbol_) : global{main[symbol_]} {}
    global(const global&) = default;

    jl_binding_t* c_binding() { return _binding; }
    generic_value value();
    function as_function();
    module as_module();

    template<
        typename T,
        typename NoRefT = std::remove_reference_t<T>,
        std::enable_if_t<!impl::is_value<NoRefT>{}
                         && !std::is_same_v<NoRefT, generic_value>>* = nullptr>
    global& operator=(T&& data_)
    {
        return *this = ::jl::value<T>{std::forward<T>(data_)};
    }

    global& operator=(const generic_value& value_)
    {
        jl_checked_assignment(_binding, value_.c_val());
        return *this;
    }

    template<typename TargT>
    operator TargT();

private:
    jl_binding_t* _binding;
};

} // namespace jl
