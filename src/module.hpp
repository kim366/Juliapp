#pragma once

#include "global.hpp"
#include "value.hpp"
#include "utility.hpp"

#include <julia/julia.h>
#include <utility>

namespace jl
{

class module : public value
{
public:
    explicit module(value val);
    explicit module(jl::from_raw_t, jl_module_t* mod);

    using value::operator=;

    // TODO: switch const char* to jl::symbol
    global operator[](const char* name) const;

    jl_module_t* raw() const;
};

} // namespace jl
