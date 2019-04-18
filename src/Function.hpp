#include <utility>

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
    function(generic_value val_) : _function{std::move(val_)} {}

    explicit function(symbol symbol_)
        : _function{jl_get_global(jl_main_module, symbol_.c_sym())}
    {
    }

    jl_function_t* c_fn() { return _function.c_val(); }
    generic_value generic();

    template<typename... ArgTs>
    generic_value operator()(ArgTs&&... args_);

    bool operator==(const function& rhs) const
    {
        return _function.c_val() == rhs._function.c_val();
    }

    bool operator!=(const function& rhs) const { return !(rhs == *this); }

private:
    generic_value _function;
};

} // namespace jl
