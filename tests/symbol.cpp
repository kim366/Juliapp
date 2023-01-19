#include "julia.hpp"
#include "utility.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

TEST_CASE("Symbol")
{
    // before each test
    constexpr auto* INPUT = "hello";
    constexpr auto* EXPECTED = ":hello";
    auto* INVALID = jl_box_uint8(19); // NOLINT(readability-magic-numbers)
    auto* DUMMY = jl_symbol("dummy");
    auto* RAW = jl_symbol(INPUT);
    auto* RAW_GENERIC = reinterpret_cast<jl_value_t*>(RAW);
    const auto NUM_BACKGROUND_ROOTED = num_rooted();

    SECTION("Raw symbol contains the given name")
    {
        const auto subject = jl::symbol{INPUT};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Wrapped generic value can be converted to symbol")
    {
        const auto wrapped = jl::value{jl::from_raw, RAW_GENERIC};

        const auto subject = jl::symbol{wrapped};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Moved wrapped generic value can be converted to symbol")
    {
        auto wrapped = jl::value{jl::from_raw, RAW_GENERIC};

        const auto subject = jl::symbol{std::move(wrapped)};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Wrapped generic value can be assigned to symbol")
    {
        const auto wrapped = jl::value{jl::from_raw, RAW_GENERIC};
        auto subject = jl::symbol{jl::from_raw, DUMMY};

        auto& result = subject = wrapped;

        REQUIRE(repr(subject) == EXPECTED);
        REQUIRE(&result == &subject);
    }

    SECTION("Moved wrapped generic value can be assigned to symbol")
    {
        auto wrapped = jl::value{jl::from_raw, RAW_GENERIC};
        auto subject = jl::symbol{jl::from_raw, DUMMY};

        auto& result = subject = std::move(wrapped);

        REQUIRE(repr(subject) == EXPECTED);
        REQUIRE(&result == &subject);
    }

    SECTION("Wrapped generic value constructor is explicit")
    {
        static_assert(!std::is_convertible_v<jl::value, jl::symbol>);
    }

    SECTION("Raw generic value can be converted to symbol")
    {
        const auto subject = jl::symbol{jl::from_raw, RAW};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Raw generic value gets rooted and freed")
    {
        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);

        {
            const auto subject = jl::symbol{jl::from_raw, RAW};
            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Wrapped generic value gets rooted and freed")
    {
        const auto wrapped = jl::value{jl::from_raw, RAW_GENERIC};

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);

        {
            const auto subject = jl::symbol{wrapped};
            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 2);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
    }

    SECTION("Moved wrapped generic value does not get re-rooted")
    {
        auto wrapped = jl::value{jl::from_raw, RAW_GENERIC};

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);

        {
            const auto subject = jl::symbol{std::move(wrapped)};
            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Invalid wrapped generic value causes error")
    {
        const auto wrapped = jl::value{jl::from_raw, INVALID};
        const auto* expected = "Expected Symbol, got UInt8";

        REQUIRE_THROWS_MATCHES(jl::symbol{wrapped}, std::invalid_argument, Catch::Matchers::Message(expected));
    }

    SECTION("Invalid wrapped generic value assignment causes error")
    {
        const auto wrapped = jl::value{jl::from_raw, INVALID};
        const auto* expected = "Expected Symbol, got UInt8";
        auto subject = jl::symbol{jl::from_raw, RAW};

        REQUIRE_THROWS_MATCHES(subject = wrapped, std::invalid_argument, Catch::Matchers::Message(expected));
    }

    SECTION("Invalid wrapped generic value gets rooted and freed")
    {
        auto wrapped = jl::value{jl::from_raw, INVALID};

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);

        REQUIRE_THROWS(jl::symbol{std::move(wrapped)});

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    // after each test
    REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED);
}

