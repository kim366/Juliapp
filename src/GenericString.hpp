#pragma once

namespace jl
{

namespace util
{

class generic_string
{
public:
    generic_string(const std::string& name_) : generic_string{name_.c_str()} {}

    generic_string(const char* name_) : _name{name_} {}

    operator const char*() { return _name; }

private:
    const char* _name;
};

} // namespace util

} // namespace jl