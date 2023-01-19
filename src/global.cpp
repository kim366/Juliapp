#include "global.hpp"

namespace jl
{

global::global(from_raw_t, jl_binding_t* val)
    : raw_{val}
{

}

const value& global::assign(const value& val)
{
    if (raw()->constp)
    {
        throw std::logic_error{std::string{"Cannot assign to constant "} + jl_symbol_name(raw()->name)};
    }

    // TODO: handle failed conversions
    auto* converted_value = jl_call2(impl::convert_fn, jl_atomic_load_relaxed(&raw()->ty), val.raw());
    jl_checked_assignment(raw_, converted_value);

    return val;
}

jl_binding_t* global::raw() const
{
    return raw_;
}

value global::operator*() const
{
    return value{from_raw, jl_atomic_load_relaxed(&raw()->value)};
}

} // namespace jl
