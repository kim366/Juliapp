#include "value.hpp"
#include "init.hpp"

namespace jl
{

value::value(jl_value_t* raw) : raw_{raw}
{
    impl::root_value(raw_);
}

value::value(const value& other)
    : value{other.raw_}
{
}

value& value::operator=(const value& other)
{
    auto tmp = other;
    std::swap(tmp.raw_, raw_);
    return *this;
}

value::value(value&& other) noexcept
    : raw_{nullptr}
{
    std::swap(other.raw_, raw_);
}

value& value::operator=(value&& other) noexcept
{
    std::swap(other.raw_, raw_);
    return *this;
}

value value::operator()() const
{
    return value::from_raw(jl_call0(raw()));
}

value::~value()
{
    if (raw_ != nullptr)
    {
        impl::release_value(raw_);
    }
}

value value::from_raw(jl_value_t* val)
{
    return value{val};
}

jl_value_t* value::raw() const
{
    return raw_;
}

} // namespace jl
