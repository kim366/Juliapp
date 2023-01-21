#pragma once

#include "assert.hpp"
#include "externs.hpp"
#include "init.hpp"
#include "value.hpp"

#include <julia.h>

namespace jl
{

class global
{
private:
    jl_binding_t* raw_;

public:
    explicit global(from_raw_t, jl_binding_t* val);

    value operator*() const;

    const value& assign(const value& val);

    jl_binding_t* raw() const;
};

} // namespace jl