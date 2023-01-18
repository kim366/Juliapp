#pragma once

#include "utility.hpp"
#include "value.hpp"

namespace jl
{

class symbol : value
{
public:
    symbol(const char* name); // NOLINT(google-explicit-constructor)
    explicit symbol(value val);
    explicit symbol(jl::from_raw_t, jl_sym_t* sym);
    jl_sym_t* raw() const; // NOLINT(modernize-use-nodiscard)
};

symbol::symbol(const char* name)
    : value{value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(name)))}
{
}

symbol::symbol(value val)
    : value{std::move(val)}
{
    if (!jl_typeis(raw(), jl_symbol_type))
    {
        const auto* type_name = jl_string_ptr(jl_call1(impl::repr_fn, jl_typeof(raw())));
        throw std::invalid_argument{std::string{"Expected Symbol, got "} + type_name};
    }
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