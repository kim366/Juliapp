#include "symbol.hpp"

#include "externs.hpp"

namespace jl
{

symbol::symbol(const char* name)
    : symbol{from_raw, jl_symbol(name)}
{
}

symbol::symbol(value val)
    : value{std::move(val), jl_symbol_type}
{
}

symbol::symbol(from_raw_t, jl_sym_t* sym)
    : value{from_raw, reinterpret_cast<jl_value_t*>(sym)}
{
}

symbol& symbol::operator=(value val)
{
    downcast_assign(std::move(val), jl_symbol_type);
    return *this;
}

jl_sym_t* symbol::raw() const
{
    return reinterpret_cast<jl_sym_t*>(value::raw());
}

} // namespace jl