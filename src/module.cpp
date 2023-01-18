#include "module.hpp"

namespace jl
{

module::module(value val)
    : value{std::move(val), jl_module_type}
{
}

module::module(jl::from_raw_t, jl_module_t* mod)
    : value{value::from_raw(reinterpret_cast<jl_value_t*>(mod))}
{
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

#ifndef JLPP_MANUAL_INIT
const module main = module{jl::from_raw, (impl::ensure_init(), jl_main_module)};
#endif
// TODO: add else

} // namespace jl
