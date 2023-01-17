#pragma once

namespace jl
{

class symbol : value
{
public:
    symbol(const char* name);
    jl_sym_t* raw() const;
};

symbol::symbol(const char* name)
    : value{value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(name)))}
{
}
jl_sym_t* symbol::raw() const
{
    return reinterpret_cast<jl_sym_t*>(value::raw());
}

} // namespace jl