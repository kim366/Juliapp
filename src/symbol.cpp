#include "symbol.hpp"

#include "externs.hpp"

namespace jl
{

symbol::symbol(const char* name)
    : value{value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(name)))}
{
}

symbol::symbol(value val)
    : value{std::move(val), jl_symbol_type}
{
}

symbol::symbol(jl::from_raw_t, jl_sym_t* sym)
    : value{value::from_raw(reinterpret_cast<jl_value_t*>(sym))}
{
}

jl_sym_t* symbol::raw() const
{
    return reinterpret_cast<jl_sym_t*>(value::raw());
}

} // namespace jl