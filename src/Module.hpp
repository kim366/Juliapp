#pragma once

#include "GenericString.hpp"

#include <julia.h>

namespace jl
{

class module
{

public:
    explicit module(util::generic_string name_)
        : _module{reinterpret_cast<jl_module_t*>(
              jl_get_global(jl_main_module, jl_symbol(name_)))}
    {
    }

    module(module& parent_module_, util::generic_string name_)
        : _module{reinterpret_cast<jl_module_t*>(
              jl_get_global(parent_module_, jl_symbol(name_)))}
    {
    }

    operator jl_module_t*() { return _module; }

    bool operator==(const module& rhs) const { return _module == rhs._module; }

    bool operator!=(const module& rhs) const { return !(rhs == *this); }

private:
    jl_module_t* _module;
};

} // namespace jl