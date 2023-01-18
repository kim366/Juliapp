#pragma once

#include <vector>
#include <julia/julia.h>

namespace jl::impl
{

extern std::vector<jl_value_t*> rooted_values;
extern jl_ptls_t ptls;
extern jl_function_t* convert_fn;
extern jl_function_t* repr_fn;

}
