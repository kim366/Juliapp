#pragma once

namespace jl
{

struct from_raw_t
{
    explicit from_raw_t() = default;
};

inline constexpr from_raw_t from_raw{};

} // namespace jl

