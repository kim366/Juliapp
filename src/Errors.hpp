#include <stdexcept>

namespace jl
{

struct language_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
    virtual ~language_error() {}
};

struct value_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
    virtual ~value_error() {}
};

} // namespace jl
