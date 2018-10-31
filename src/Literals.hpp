#pragma once

#include "Function.hpp"

namespace jl::literals
{
function operator"" _jlf(const char* name_, std::size_t)
{
    return function{name_};
}

module operator"" _jlm(const char* name_, std::size_t)
{
    return module{name_};
}

} // namespace jl::literals