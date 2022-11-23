#pragma once

#include "value.hpp"
#include "global.hpp"
#include "function.hpp"

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

inline function operator""_jlf(const char* name, std::size_t)
{
    return function{*main[name]};
}

} // namespace jl::literals