#pragma once

#include "assert.hpp"
#include "externs.hpp"
#include "init.hpp"
#include "value.hpp"

#include <julia/julia.h>

namespace jl
{

class global
{
private:
    jl_binding_t* raw_;
    bool is_writeable_;

public:
    value operator*() const;

    void set(const value& val);

    jl_binding_t* raw() const;
    static global from_raw(jl_binding_t* val, bool is_writeable = false);

private:
    explicit global(jl_binding_t* val, bool is_writeable);
};

void global::set(const value& val)
{
    // TODO: replace with exception
    impl_jlpp_assert("global is not writeable (const or owned by a different module)", is_writeable_);
    const auto converted_value = jl_call2(impl::convert_fn, jl_atomic_load_relaxed(&raw()->ty), val.raw());
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