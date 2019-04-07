#pragma once

#include <string>

namespace jl
{

namespace util
{

class string_view
{
public:
    string_view(const std::string& name_) : string_view{name_.c_str()} {}

    string_view(const char* name_) : _name{name_} {}

    operator const char*() const { return _name; }

private:
    const char* _name;
};

} // namespace util

} // namespace jl