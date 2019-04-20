#include <utility>

#pragma once

#include "Errors.hpp"
#include "Symbol.hpp"
#include "Value.hpp"

#include <julia.h>

namespace jl
{

class global;

class module
{

public:
    module(generic_value val_) : _module{std::move(val_)} {}

    explicit module(symbol name_)
        : _module{jl_get_global(jl_main_module, name_.c_sym())}
    {
    }

    global operator[](symbol name_);

    jl_module_t* c_mod() const
    {
        return reinterpret_cast<jl_module_t*>(_module.c_val());
    }

    bool operator==(const module& rhs) const
    {
        return _module.c_val() == rhs._module.c_val();
    }

    bool operator!=(const module& rhs) const { return !(rhs == *this); }

private:
    generic_value _module;
};

inline module main =
    generic_value{reinterpret_cast<jl_value_t*>(jl_main_module)};

} // namespace jl