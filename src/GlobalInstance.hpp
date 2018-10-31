#pragma once

#include "Sync.hpp"

#include <julia.h>

namespace jl::impl
{

struct global
{
    global() { jl_init(); }

    ~global()
    {
        jl_atexit_hook(0);
        delete[] impl::synced_cpp_types;
        delete[] impl::synced_jl_types;
    }
} const volatile global_instance;

} // namespace jl::impl
