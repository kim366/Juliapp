#pragma once

#include "global.hpp"
#include "value.hpp"
#include <julia/julia.h>
#include <utility>

namespace jl
{

class module
{
private:
    jl_module_t* raw_;

public:
    explicit module(const value& val);

    // TODO: switch const char* to jl::symbol
    global operator[](const char* name) const;

    static module from_raw(jl_module_t* raw);

private:
    module(jl_module_t* raw);
};

module::module(jl_module_t* raw)
    : raw_{raw}
{
}

module::module(const value& val)
{
    if (!jl_is_module(val.raw()))
        jl_error("Value is not a module");

    raw_ = reinterpret_cast<jl_module_t*>(val.raw());
}

global module::operator[](const char* name) const
{
    return global::from_raw(jl_get_binding_wr(raw_, jl_symbol(name), 0));
}

module module::from_raw(jl_module_t* raw)
{
    return module{raw};
}

#ifndef JLPP_MANUAL_INIT
const auto main = module::from_raw(
    (impl::ensure_init(), jl_main_module));
#endif

} // namespace jl
