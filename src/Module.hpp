#pragma once

#include "StringView.hpp"

#include <julia.h>

namespace jl
{

class module
{

public:
    explicit module(util::string_view name_)
        : _module{reinterpret_cast<jl_module_t*>(
              jl_get_global(jl_main_module, jl_symbol(name_)))}
    {
    }

    module(module& parent_module_, util::string_view name_)
        : _module{reinterpret_cast<jl_module_t*>(
              jl_get_global(parent_module_.c_mod(), jl_symbol(name_)))}
    {
    }

    jl_module_t* c_mod() { return _module; }

    bool operator==(const module& rhs) const { return _module == rhs._module; }

    bool operator!=(const module& rhs) const { return !(rhs == *this); }

private:
    jl_module_t* _module;
};

} // namespace jl