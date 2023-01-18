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

} // namespace jl
