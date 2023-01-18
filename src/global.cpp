#include "global.hpp"

namespace jl
{

void global::set(const value& val)
{
    // TODO: replace with exception
    impl_jlpp_assert("global is not writeable (const or owned by a different module)", is_writeable_);
    auto* converted_value = jl_call2(impl::convert_fn, jl_atomic_load_relaxed(&raw()->ty), val.raw());
    jl_checked_assignment(raw_, converted_value);
}

jl_binding_t* global::raw() const
{
    return raw_;
}

global::global(jl_binding_t* val, bool is_writeable)
    : raw_{val}
    , is_writeable_{is_writeable}
{
}

global global::from_raw(jl_binding_t* val, bool is_writeable)
{
    return global{val, is_writeable};
}

value global::operator*() const
{
    // TODO: replace with jl_binding_value once available
    return value::from_raw(jl_atomic_load_relaxed(&raw()->value));
}

} // namespace jl
