#pragma once

#include <cassert>
#include <stdexcept>

#ifdef JLPP_IMPL_UNIT_TESTING

namespace jl
{

namespace test
{

struct failed_assertion : std::logic_error
{
    using std::logic_error::logic_error;

    virtual ~failed_assertion() noexcept {}
};

struct failed_static_assertion : std::logic_error
{
    using std::logic_error::logic_error;

    virtual ~failed_static_assertion() noexcept {}
};

} // namespace test

namespace impl::test
{

struct assertion_instance
{
    assertion_instance(bool expr_)
    {
        if (!expr_)
            throw jl::test::failed_assertion{"Assertion failed"};
    }
};

template<bool>
struct static_assertion_instance;

template<>
struct static_assertion_instance<true>
{
    static_assertion_instance(const char*) {}
};

template<>
struct static_assertion_instance<false>
{
    static_assertion_instance(const char* reason_)
    {
        throw jl::test::failed_static_assertion{reason_};
    }
};
} // namespace impl::test

} // namespace jl

// clang-format off
#define jlpp_assert(expr)                                                      \
    jl::impl::test::assertion_instance{expr}
#define jlpp_static_assert(expr, ...)                                          \
    jl::impl::test::static_assertion_instance<expr>{#__VA_ARGS__}
// clang-format on

#else
#define jlpp_assert(expr) assert(expr)
#define jlpp_static_assert(expr, ...) static_assert(expr, __VA_ARGS__)
#endif