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

} // namespace jl