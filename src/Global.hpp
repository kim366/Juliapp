#pragma once

#include <Helpers.hpp>
#include <julia.h>
#include <utility>

namespace jl
{

class symbol;
class generic_value;
struct function;
struct module;

class global
{
public:
    global(jl_binding_t* binding_);
    global(symbol symbol_);
    global(const global&) = default;

    jl_binding_t* c_binding();
    generic_value value();
    function as_function();
    module as_module();

    template<
        typename T,
        typename NoRefT = std::remove_reference_t<T>,
        std::enable_if_t<!impl::is_value<NoRefT>{}
                         && !std::is_same_v<NoRefT, generic_value>>* = nullptr>
    global& operator=(T&& data_);

    global& operator=(const generic_value& value_);

    template<typename TargT>
    operator TargT();

private:
    jl_binding_t* _binding;
};

} // namespace jl
