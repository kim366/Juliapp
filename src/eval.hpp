#pragma once

namespace jl
{

void eval(const char* source)
{
    jl_eval_string(source);
}

} // namespace jl
