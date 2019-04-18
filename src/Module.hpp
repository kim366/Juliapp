#pragma once

#include "Errors.hpp"
#include "Global.hpp"
#include "Symbol.hpp"
#include "Value.hpp"

#include <julia.h>

namespace jl
{

class module
{

public:
    module(generic_value val_) : _module{val_} {}

    explicit module(symbol name_)
        : _module{jl_get_global(jl_main_module, name_.c_sym())}
    {
    }

    global operator[](symbol name_)
    {
        jl_binding_t* binding = nullptr;

        JL_TRY { binding = jl_get_binding_wr(c_mod(), name_.c_sym(), true); }
        JL_CATCH { throw language_error{"Failed to get binding to symbol"}; }

        return binding;
    }

    jl_module_t* c_mod() const
    {
        return reinterpret_cast<jl_module_t*>(_module.c_val());
    }
    generic_value genric();

    bool operator==(const module& rhs) const
    {
        return _module.c_val() == rhs._module.c_val();
    }

    bool operator!=(const module& rhs) const { return !(rhs == *this); }

private:
    generic_value _module;
};

} // namespace jl