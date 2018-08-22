#pragma once

#include <stdexcept>

namespace jl
{

struct error : std::runtime_error
{
    using std::runtime_error::runtime_error;
    virtual ~error() = 0;
};

error::~error() {}

struct language_error : error
{
    using error::error;
    virtual ~language_error() {}
};

struct result_type_error : error
{
    using error::error;
    virtual ~result_type_error() {}
};

struct load_error : error
{
    using error::error;
    virtual ~load_error() {}
};

} // namespace jl
