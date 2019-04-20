#include <utility>

#pragma once

#include "Errors.hpp"
#include "Symbol.hpp"
#include "Value.hpp"

#include <julia.h>

namespace jl
{

class global;

struct module : generic_value
{
    module(generic_value val_) : generic_value{std::move(val_)} {}

    explicit module(symbol name_)
        : generic_value{jl_get_global(jl_main_module, name_.c_sym())}
    {
    }

    global operator[](symbol name_);

    jl_module_t* c_mod() const
    {
        return reinterpret_cast<jl_module_t*>(c_val());
    }

    bool operator==(const module& rhs) const { return c_val() == rhs.c_val(); }

    bool operator!=(const module& rhs) const { return !(rhs == *this); }
};

inline module main =
    generic_value{reinterpret_cast<jl_value_t*>(jl_main_module)};

} // namespace jl