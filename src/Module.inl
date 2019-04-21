#pragma once

#include "Errors.hpp"
#include "Symbol.hpp"

#include <Global.hpp>
#include <Module.hpp>

namespace jl
{

inline module::module(generic_value val_) : generic_value{std::move(val_)} {}

inline module::module(symbol name_)
    : generic_value{jl_get_global(jl_main_module, name_.c_sym())}
{
}

inline global module::operator[](symbol name_) const
{
    return impl::get_binding(c_mod(), name_);
}

inline jl_module_t* module::c_mod() const
{
    return reinterpret_cast<jl_module_t*>(c_val());
}

inline bool module::operator==(const module& rhs) const
{
    return c_val() == rhs.c_val();
}

inline bool module::operator!=(const module& rhs) const
{
    return !(rhs == *this);
}

namespace impl
{

inline global get_binding(jl_module_t* mod, symbol name_)
{
    jl_binding_t* binding = nullptr;

    JL_TRY { binding = jl_get_binding_wr(mod, name_.c_sym(), true); }
    JL_CATCH { throw language_error{"Failed to get binding to symbol"}; }

    return binding;
}

} // namespace impl

} // namespace jl