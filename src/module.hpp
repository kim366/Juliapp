#pragma once

#include "global.hpp"
#include "value.hpp"
#include "utility.hpp"
#include "symbol.hpp"

#include <julia/julia.h>
#include <utility>

namespace jl
{

class module : public value
{
public:
    explicit module(value val);
    explicit module(from_raw_t, jl_module_t* mod);
    module& operator=(value val);

    global operator[](const symbol& sym) const;

    jl_module_t* raw() const;
};

} // namespace jl
