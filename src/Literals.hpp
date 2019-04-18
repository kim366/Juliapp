#pragma once

#include "Function.hpp"
#include "Global.hpp"

namespace jl::literals
{
inline function operator""_jlf(const char* name_, std::size_t)
{
    return function{name_};
}

inline module operator""_jlm(const char* name_, std::size_t)
{
    return module{name_};
}

inline global operator""_jlg(const char* name_, std::size_t)
{
    return global{name_};
}

} // namespace jl::literals