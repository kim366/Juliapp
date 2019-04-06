#pragma once

#include "Sync.hpp"

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
    jl_atexit_hook(status_);
    delete[] synced_cpp_types;
    delete[] synced_jl_types;
    rooted_values.clear();
    rooted_values.shrink_to_fit();
}

inline void root_value(jl_value_t* val)
{
    rooted_values.push_back(val);
}

inline void release_value(jl_value_t* val)
{
    const auto found_val =
        std::find(rooted_values.rbegin(), rooted_values.rend(), val);
    jlpp_assert(found_val != rooted_values.rend()
                && "Releasing unrooted value");
    rooted_values.erase(std::next(found_val).base());
}

class global
{
public:
    global() noexcept { init(); }
    ~global() { quit(); }
};
#ifndef JLPP_MANUAL_INIT
inline global global_instance{};
#endif

inline void root_scanner_cb(int)
{
    for (jl_value_t* val : rooted_values)
        jl_gc_mark_queue_obj(jl_get_ptls_states(), val);
}

} // namespace jl::impl
