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
    explicit symbol(from_raw_t, jl_sym_t* sym);
    symbol& operator=(value val);

    jl_sym_t* raw() const;
};

} // namespace jl