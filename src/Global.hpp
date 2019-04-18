#pragma once

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

    jl_binding_t* c_binding() { return _binding; }
    generic_value value();
    function as_function();
    module as_module();

    template<typename TargT>
    operator TargT();

private:
    jl_binding_t* _binding;
};

} // namespace jl
