#pragma once

#include "GenericString.hpp"

#include <julia.h>
#include <string>

namespace jl
{

class module
{

public:
    explicit module(generic_string name_)
        : _module{reinterpret_cast<jl_module_t*>(
              jl_get_global(jl_main_module, jl_symbol(name_)))}
    {
    }

    module(module& parent_module_, generic_string name_)
        : _module{reinterpret_cast<jl_module_t*>(
              jl_get_global(parent_module_, jl_symbol(name_)))}
    {
    }

    operator jl_module_t*() { return _module; }

private:
    jl_module_t* _module;
};

class function
{
public:
    explicit function(generic_string name_)
        : _function{jl_get_function(jl_main_module, name_)}
    {
    }

    function(module module_, generic_string name_)
        : _function{jl_get_function(module_, name_)}
    {
    }

    operator jl_function_t*() { return _function; }
    template<typename... ArgTs>
    class value operator()(ArgTs&&... args_);

private:
    jl_function_t* _function;
};

} // namespace jl
