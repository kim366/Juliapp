#pragma once

#include <stdexcept>

namespace jl
{

struct error : std::runtime_error
{
protected:
    using std::runtime_error::runtime_error;

public:
    ~error() override = default;
};

struct language_error : error
{
    using error::error;
    ~language_error() override = default;
};

struct result_type_error : error
{
    using error::error;
    ~result_type_error() override = default;
};

struct load_error : error
{
    using error::error;
    ~load_error() override = default;
};

} // namespace jl
