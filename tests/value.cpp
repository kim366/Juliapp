#include "julia.hpp"
#include "utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Value")
{
    // before each test
    const auto NUM_BACKGROUND_ROOTED = num_rooted();
    auto* INPUT = jl_box_uint8(19);

    SECTION("Wrapping value gives access to raw value")
    {
        auto* expected = INPUT;
        const auto subject = jl::value{jl::from_raw, INPUT};

        auto* result = subject.raw();

        REQUIRE(result == expected);
    }

    SECTION("Wrapping value roots it")
    {
        const auto subject = jl::value{jl::from_raw, INPUT};

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
    }

    SECTION("Wrapped value is freed upon destruction")
    {
        {
            const auto subject = jl::value{jl::from_raw, INPUT};
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Copying wrapped value keeps underlying raw value")
    {
        auto* expected = INPUT;
        const auto subject = jl::value{jl::from_raw, INPUT};

        const auto copy = subject; // NOLINT(performance-unnecessary-copy-initialization)

        REQUIRE(copy.raw() == expected);
    }

    SECTION("Moving wrapped value keeps underlying raw value")
    {
        auto* expected = INPUT;
        auto subject = jl::value{jl::from_raw, INPUT};

        const auto copy = std::move(subject);

        REQUIRE(copy.raw() == expected);
    }

    SECTION("Copying wrapped value roots new value")
    {
        const auto subject = jl::value{jl::from_raw, INPUT};

        {
            const auto copy = subject; // NOLINT(performance-unnecessary-copy-initialization)

            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 2);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
    }

    SECTION("Moving wrapped does not re-root")
    {
        auto subject = jl::value{jl::from_raw, INPUT};

        {
            const auto copy = std::move(subject);

            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }
}
