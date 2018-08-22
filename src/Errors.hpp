#pragma once

#include <stdexcept>

namespace jl
{

struct language_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
    virtual ~language_error() {}
};

struct value_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
    virtual ~value_error() {}
};

struct load_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
    virtual ~load_error() {}
};

} // namespace jl
