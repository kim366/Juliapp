#pragma once

#include "value.hpp"
#include "global.hpp"

namespace jl::literals
{

inline global operator""_jlg(const char* name, std::size_t)
{
    return main[name];
}

inline value operator""_jlv(const char* name, std::size_t)
{
    return *main[name];
}

} // namespace jl::literals