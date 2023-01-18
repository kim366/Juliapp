#include "julia.hpp"
#include "utility.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

constexpr static auto* INPUT = "hello";
constexpr static auto* EXPECTED = ":hello";

TEST_CASE("Raw symbol contains the given name", "[symbol]")
{
    const auto subject = jl::symbol{INPUT};

    const auto result = repr(subject);

    REQUIRE(result == EXPECTED);
}

TEST_CASE("Wrapped generic value can be converted to symbol", "[symbol]")
{
    const auto wrapped = jl::value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(INPUT)));
    const auto subject = jl::symbol{wrapped};

    const auto result = repr(subject);

    REQUIRE(result == EXPECTED);
}

TEST_CASE("Moved wrapped generic value can be converted to symbol", "[symbol]")
{
    auto wrapped = jl::value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(INPUT)));
    const auto subject = jl::symbol{std::move(wrapped)};

    const auto result = repr(subject);

    REQUIRE(result == EXPECTED);
}

TEST_CASE("Wrapped generic value constructor is explicit", "[symbol]")
{
    static_assert(!std::is_convertible_v<jl::value, jl::symbol>);
}

TEST_CASE("Raw generic value can be converted to symbol", "[symbol]")
{
    auto* raw = jl_symbol(INPUT);
    const auto subject = jl::symbol{jl::from_raw, raw};

    const auto result = repr(subject);

    REQUIRE(result == EXPECTED);
}

TEST_CASE("Raw generic value gets rooted and freed", "[symbol]")
{
    const auto num_background_rooted = jl::impl::rooted_values.size();
    auto* raw = jl_symbol(INPUT);

    REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 0);

    {
        const auto subject = jl::symbol{jl::from_raw, raw};
        REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 1);
    }

    REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 0);
}

TEST_CASE("Wrapped generic value gets rooted and freed", "[symbol]")
{
    const auto num_background_rooted = jl::impl::rooted_values.size();
    const auto wrapped = jl::value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(INPUT)));

    REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 1);

    {
        const auto subject = jl::symbol{wrapped};
        REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 2);
    }

    REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 1);
}

TEST_CASE("Moved wrapped generic value does not get re-rooted", "[symbol]")
{
    const auto num_background_rooted = jl::impl::rooted_values.size();
    auto wrapped = jl::value::from_raw(reinterpret_cast<jl_value_t*>(jl_symbol(INPUT)));

    REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 1);

    {
        const auto subject = jl::symbol{std::move(wrapped)};
        REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 1);
    }

    REQUIRE(jl::impl::rooted_values.size() == num_background_rooted + 0);
}

TEST_CASE("Invalid wrapped generic value causes error", "[symbol]")
{
    const auto wrapped = jl::value::from_raw(jl_box_uint8(19));
    const auto* expected = "Expected Symbol, got UInt8";

    REQUIRE_THROWS_MATCHES(jl::symbol{wrapped}, std::invalid_argument, Catch::Matchers::Message(expected));
}
