#include "value.hpp"

#include "externs.hpp"
#include "init.hpp"

namespace jl
{


value::value(from_raw_t, jl_value_t* raw)
    : raw_{raw}
{
    impl::root_value(raw_);
}

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

static std::string datatype_name(jl_value_t* type)
{
    return jl_string_ptr(jl_call1(impl::repr_fn, type));
}

static void expect_admissible_downcast(jl_value_t* raw, jl_datatype_t* expected_type)
{
    if (!jl_typeis(raw, expected_type))
    {
        const auto type_name = datatype_name(reinterpret_cast<jl_value_t*>(expected_type));
        const auto other_type_name = datatype_name(jl_typeof(raw));
        throw std::invalid_argument{std::string{"Expected "} + type_name + ", got " + other_type_name};
    }
}

value::value(value&& val, jl_datatype_t* expected_type)
// thanks to the call to another constructor here, destructors get properly called in case of throw
    : value{std::move(val)}
{
    expect_admissible_downcast(raw(), expected_type);
}

// assignment has strong exception guarantee
void value::downcast_assign(value&& val, jl_datatype_t* expected_type)
{
    expect_admissible_downcast(val.raw(), expected_type);
    *this = std::move(val);
}

} // namespace jl