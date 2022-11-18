#pragma once

#include "value.hpp"
#include "global.hpp"

namespace jl::literals
{

inline global operator""_jlg(const char* name_, std::size_t)
{
    return jl::main[name_];
}

inline value operator""_jlv(const char* name_, std::size_t)
{
    return *jl::main[name_];
}

} // namespace jl::literals