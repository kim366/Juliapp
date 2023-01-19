#include "eval.hpp"

namespace jl
{

value eval(const char* source)
{
    return value{from_raw, jl_eval_string(source)};
}

} // namespace jl
