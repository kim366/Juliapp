#pragma once

#include "Assert.hpp"

#include <algorithm>
#include <julia.h>
#include <julia_gcext.h>
#include <vector>

namespace jl::impl
{

inline std::vector<jl_value_t*> rooted_values;

void root_scanner_cb(int);

inline void init()
{
    jl_init();
    jl_gc_set_cb_root_scanner(root_scanner_cb, true);
}

inline void quit(int status_ = 0)
{
    rooted_values.clear();
    rooted_values.shrink_to_fit();
    jl_atexit_hook(status_);
}

inline void root_value(jl_value_t* val)
{
    rooted_values.push_back(val);
}

inline void release_value(jl_value_t* val)
{
    if (val != nullptr)
    {
        const auto found_val =
            std::find(rooted_values.rbegin(), rooted_values.rend(), val);
        jlpp_assert(found_val != rooted_values.rend()
                    && "Releasing unrooted value");
        rooted_values.erase(std::next(found_val).base());
    }
}

void ensure_init();

class init_guard
{
    init_guard() noexcept { init(); }
    ~init_guard() { quit(); }

    friend void impl::ensure_init();
};

inline void ensure_init()
{
#ifndef JLPP_MANUAL_INIT
    static const auto guard = init_guard{};
#endif
}

inline void root_scanner_cb(int)
{
    for (jl_value_t* val : rooted_values)
        jl_gc_mark_queue_obj(jl_get_ptls_states(), val);
}

} // namespace jl::impl
