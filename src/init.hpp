#pragma once

#include "assert.hpp"

#include <algorithm>
#include <julia/julia.h>
#include <julia/julia_gcext.h>
#include <vector>

namespace jl::impl
{

void root_scanner_cb(int);
void init();
void quit(int status = 0);
void root_value(jl_value_t* val);
void release_value(jl_value_t* val);

class init_guard
{
    init_guard() noexcept;
    ~init_guard();

    friend void ensure_init();
};

void ensure_init();

} // namespace jl::impl
