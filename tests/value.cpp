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
}
