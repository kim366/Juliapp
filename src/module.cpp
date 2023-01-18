#include "module.hpp"

namespace jl
{

module::module(const value& val)
    : value{val}
{
    if (!jl_is_module(raw()))
    {
        jl_error("Value is not a module");
    }
}

module::module(value&& val)
    : value{std::move(val)}
{
    if (!jl_is_module(raw()))
    {
        jl_error("Value is not a module");
    }
}

jl_module_t* module::raw() const
{
    return reinterpret_cast<jl_module_t*>(value::raw());
}

global module::operator[](const char* name) const
{
    auto* sym = jl_symbol(name);
    auto* binding = jl_get_binding(raw(), sym);
    auto is_writeable = !binding->constp && binding->owner == raw();

    return global::from_raw(binding, is_writeable);
}

module module::from_raw(jl_module_t* raw)
{
    return value::from_raw(reinterpret_cast<jl_value_t*>(raw));
}

#ifndef JLPP_MANUAL_INIT
const module main = module::from_raw(
    (impl::ensure_init(), jl_main_module));
#endif
// TODO: add else

} // namespace jl
