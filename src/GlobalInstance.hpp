#pragma once

#include "Sync.hpp"

#include <algorithm>
#include <julia.h>
#include <julia_gcext.h>
#include <vector>

namespace jl::impl
{

void root_scanner_cb(int);

class global
{
public:
    global() noexcept
    {
        jl_init();
        jl_gc_set_cb_root_scanner(root_scanner_cb, true);
    }

    ~global()
    {
        jl_atexit_hook(0);
        delete[] impl::synced_cpp_types;
        delete[] impl::synced_jl_types;
    }

    void root_value(jl_value_t* val) { rooted_values.push_back(val); }

    void release_value(jl_value_t* val)
    {
        const auto found_val =
            std::find(rooted_values.rbegin(), rooted_values.rend(), val);
        jlpp_assert(found_val != rooted_values.rend()
                    && "Releasing unrooted value");
        rooted_values.erase(std::next(found_val).base());
    }

private:
    std::vector<jl_value_t*> rooted_values;

    friend void root_scanner_cb(int);
} inline global_instance;

inline void root_scanner_cb(int)
{
    for (jl_value_t* val : global_instance.rooted_values)
        jl_gc_mark_queue_obj(jl_get_ptls_states(), val);
}

} // namespace jl::impl
