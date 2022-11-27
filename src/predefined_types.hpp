#pragma once

#include <julia/julia.h>
#include <cstdint>

namespace jl
{

// as per https://github.com/JuliaLang/julia/blob/88a0627003c45ddac304b7be933c93caae8ae6b3/src/builtins.c#L2064
#ifdef _P64
using int_t = std::int64_t;
using uint_t = std::uint64_t;
#else
using int_t = std::int32_t;
using uint_t = std::uint32_t;
#endif

} // namespace jl