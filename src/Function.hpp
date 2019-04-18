#pragma once

#include "Module.hpp"
#include "StringView.hpp"

#include <julia.h>
#include <string>

namespace jl
{

class generic_value;

class function
{
public:
    function(jl_function_t* _function) : _function{_function} {}

    explicit function(util::string_view name_)
        : _function{jl_get_function(jl_main_module, name_)}
    {
    }

    function(module module_, util::string_view name_)
        : _function{jl_get_function(module_.c_mod(), name_)}
    {
    }

    jl_function_t* c_fn() { return _function; }

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
