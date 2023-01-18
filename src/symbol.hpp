#pragma once

#include "utility.hpp"
#include "value.hpp"

namespace jl
{

class symbol : value
{
public:
    symbol(const char* name);
    explicit symbol(value val);
    explicit symbol(jl::from_raw_t, jl_sym_t* sym);
    jl_sym_t* raw() const;
};

} // namespace jl