#pragma once

#include "global.hpp"
#include "value.hpp"
#include <julia/julia.h>
#include <utility>

namespace jl
{

class module : public value
{
public:
    module(const value& val);
    module(value&& val);

    using value::operator=;

    // TODO: switch const char* to jl::symbol
    global operator[](const char* name) const;

    jl_module_t* raw() const;
    static module from_raw(jl_module_t* raw);
};

module::module(const value& val)
    : value{val}
{
    if (!jl_is_module(raw()))
        jl_error("Value is not a module");
}

module::module(value&& val)
    : value{std::move(val)}
{
    if (!jl_is_module(raw()))
        jl_error("Value is not a module");
}

jl_module_t* module::raw() const
{
    return reinterpret_cast<jl_module_t*>(value::raw());
}

global module::operator[](const char* name) const
{
    return global::from_raw(jl_get_binding_wr(raw(), jl_symbol(name), 0));
}

module module::from_raw(jl_module_t* raw)
{
    return value::from_raw(reinterpret_cast<jl_value_t*>(raw));
}

#ifndef JLPP_MANUAL_INIT
const auto main = module::from_raw(
    (impl::ensure_init(), jl_main_module));
#endif

} // namespace jl
