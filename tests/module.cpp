#include "julia.hpp"
#include "utility.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

TEST_CASE("Module")
{
    // before each test
    constexpr auto* EXPECTED = "Base";
    auto* INVALID = jl_box_uint8(19); // NOLINT(readability-magic-numbers)
    auto* const DUMMY = jl_core_module;
    auto* const RAW = jl_base_module;
    auto* const RAW_GENERIC = reinterpret_cast<jl_value_t*>(RAW);
    const auto NUM_BACKGROUND_ROOTED = num_rooted();


    SECTION("Wrapped generic value can be converted to module")
    {
        const auto wrapped = jl::value::from_raw(RAW_GENERIC);

        const auto subject = jl::module{wrapped};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Moved wrapped generic value can be converted to module")
    {
        auto wrapped = jl::value::from_raw(RAW_GENERIC);

        const auto subject = jl::module{std::move(wrapped)};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Wrapped generic value can be assigned to module")
    {
        const auto wrapped = jl::value::from_raw(RAW_GENERIC);
        auto subject = jl::module{jl::from_raw, DUMMY};

        auto& result = subject = wrapped;

        REQUIRE(repr(subject) == EXPECTED);
        REQUIRE(&result == &subject);
    }

    SECTION("Moved wrapped generic value can be assigned to module")
    {
        auto wrapped = jl::value::from_raw(RAW_GENERIC);
        auto subject = jl::module{jl::from_raw, DUMMY};

        auto& result = subject = std::move(wrapped);

        REQUIRE(repr(subject) == EXPECTED);
        REQUIRE(&result == &subject);
    }

    SECTION("Wrapped generic value constructor is explicit")
    {
        static_assert(!std::is_convertible_v<jl::value, jl::module>);
    }

    SECTION("Raw generic value can be converted to module")
    {
        const auto subject = jl::module{jl::from_raw, RAW};

        REQUIRE(repr(subject) == EXPECTED);
    }

    SECTION("Raw generic value gets rooted and freed")
    {
        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);

        {
            const auto subject = jl::module{jl::from_raw, RAW};
            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Wrapped generic value gets rooted and freed")
    {
        const auto wrapped = jl::value::from_raw(RAW_GENERIC);

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);

        {
            const auto subject = jl::module{wrapped};
            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 2);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
    }

    SECTION("Moved wrapped generic value does not get re-rooted")
    {
        auto wrapped = jl::value::from_raw(RAW_GENERIC);

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);

        {
            const auto subject = jl::module{std::move(wrapped)};
            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Invalid wrapped generic value causes error")
    {
        const auto wrapped = jl::value::from_raw(INVALID);
        const auto* expected = "Expected Module, got UInt8";

        REQUIRE_THROWS_MATCHES(jl::module{wrapped}, std::invalid_argument, Catch::Matchers::Message(expected));
    }

    SECTION("Invalid wrapped generic value assignment causes error")
    {
        const auto wrapped = jl::value::from_raw(INVALID);
        const auto* expected = "Expected Module, got UInt8";
        auto subject = jl::module{jl::from_raw, RAW};

        REQUIRE_THROWS_MATCHES(subject = wrapped, std::invalid_argument, Catch::Matchers::Message(expected));
    }

    SECTION("Invalid wrapped generic value gets rooted and freed")
    {
        auto wrapped = jl::value::from_raw(INVALID);

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);

        REQUIRE_THROWS(jl::module{std::move(wrapped)});

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Binding resolves to requested value")
    {
        const auto input = jl::symbol{"Int64"};
        const auto* expected = "Int64";
        const auto subject = jl::module(jl::from_raw, jl_base_module);

        const auto binding = subject[input];

        auto* result = jl_atomic_load_relaxed(&binding.raw()->value);
        REQUIRE(repr(result) == expected);
    }

    SECTION("jl::main refers to the main module")
    {
        const auto* expected = "Main";
        const auto subject = jl::main;

        REQUIRE(repr(subject) == expected);
    }

    // after each test
    REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED);
}
