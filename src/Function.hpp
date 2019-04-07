#pragma once

#include "Any.hpp"
#include "Module.hpp"
#include "StringView.hpp"

#include <julia.h>
#include <string>

namespace jl
{

template<typename>
class value;

class function
{
public:
    explicit function(util::string_view name_)
        : _function{jl_get_function(jl_main_module, name_)}
    {
    }

    function(module module_, util::string_view name_)
        : _function{jl_get_function(module_, name_)}
    {
    }

    operator jl_function_t*() { return _function; }
    template<typename... ArgTs>
    value<any> operator()(ArgTs&&... args_);

    bool operator==(const function& rhs) const
    {
        return _function == rhs._function;
    }

    bool operator!=(const function& rhs) const { return !(rhs == *this); }

private:
    jl_function_t* _function;
};

} // namespace jl
