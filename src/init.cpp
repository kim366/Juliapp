#include "init.hpp"

namespace jl::impl
{

std::vector<jl_value_t*> rooted_values;
jl_ptls_t ptls;
jl_function_t* convert_fn;
jl_function_t* repr_fn;

void init()
{
    jl_init();
    ptls = static_cast<jl_ptls_t>(jl_get_ptls_states());
    convert_fn = jl_get_function(jl_base_module, "convert");
    repr_fn = jl_get_function(jl_base_module, "repr");
    jl_gc_set_cb_root_scanner(root_scanner_cb, 1);
}

void quit(int status)
{
    rooted_values = {};
    jl_atexit_hook(status);
}

void root_value(jl_value_t* val)
{
    rooted_values.push_back(val);
}

void release_value(jl_value_t* val)
{
    // TODO: replace with swap(itr, vals.back()); vals.pop_back();
    // + store address of value wrapper, ignore release of unrooted values
    // this will happen if a pointer struct field is set from within Julia
    // and then released/overwritten from C++
    // do a check that this indeed happens only from struct-pointer-fields
    // bool maybe_unrooted
    if (val != nullptr)
    {
        const auto found_val =
            std::find(rooted_values.rbegin(), rooted_values.rend(), val);
        impl_jlpp_assert("Releasing unrooted value", found_val != rooted_values.rend());
        rooted_values.erase(std::next(found_val).base());
    }
}

init_guard::init_guard() noexcept
{
    init();
}

init_guard::~init_guard()
{
    quit();
}

void ensure_init()
{
#ifndef JLPP_MANUAL_INIT
    static const auto guard = init_guard{};
#endif
}

void root_scanner_cb(int)
{
    for (jl_value_t* val : rooted_values)
    {
        jl_gc_mark_queue_obj(ptls, val);
    }
}

} // namespace jl::impl