#pragma once

#include <GenericValue.hpp>
#include <Init.hpp>
#include <julia.h>
#include <utility>

namespace jl
{

class symbol;
class global;

struct module : generic_value
{
    module(generic_value val_);
    module(symbol name_);

    global operator[](symbol name_) const;

    jl_module_t* c_mod() const;

    bool operator==(const module& rhs) const;
    bool operator!=(const module& rhs) const;
};

namespace impl
{

global get_binding(jl_module_t* mod, symbol name_);

} // namespace impl

#ifndef JLPP_MANUAL_INIT
const module main = generic_value{
    (impl::ensure_init(), reinterpret_cast<jl_value_t*>(jl_main_module))};
#endif

} // namespace jl