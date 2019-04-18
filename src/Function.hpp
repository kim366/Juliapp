#pragma once

#include "Module.hpp"
#include "Symbol.hpp"

#include <julia.h>
#include <string>

namespace jl
{

class generic_value;

class function
{
public:
    function(jl_function_t* _function) : _function{_function} {}

    explicit function(symbol symbol_)
        : _function{jl_get_global(jl_main_module, symbol_.c_sym())}
    {
    }

    jl_function_t* c_fn() { return _function; }
    generic_value genric();

    template<typename... ArgTs>
    generic_value operator()(ArgTs&&... args_);

    bool operator==(const function& rhs) const
    {
        return _function == rhs._function;
    }

    bool operator!=(const function& rhs) const { return !(rhs == *this); }

private:
    jl_function_t* _function;
};

} // namespace jl
