#pragma once

#include "Errors.hpp"
#include "Global.hpp"
#include "StringView.hpp"

#include <julia.h>

namespace jl
{

class module
{

public:
    module(jl_module_t* module_) : _module{module_} {}

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

    global operator[](util::string_view name_) const
    {
        jl_binding_t* binding = nullptr;

        JL_TRY { binding = jl_get_binding_wr(_module, jl_symbol(name_), true); }
        JL_CATCH { throw language_error{"Failed to get binding to symbol"}; }

        return binding;
    }

    jl_module_t* c_mod() { return _module; }
    generic_value genric();

    bool operator==(const module& rhs) const { return _module == rhs._module; }

    bool operator!=(const module& rhs) const { return !(rhs == *this); }

private:
    jl_module_t* _module;
};

} // namespace jl