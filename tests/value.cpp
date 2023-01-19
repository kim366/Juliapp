#include "julia.hpp"
#include "utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Value")
{
    // before each test
    const auto NUM_BACKGROUND_ROOTED = num_rooted();
    auto* DUMMY = jl_nothing;
    auto* INPUT = jl_box_uint8(19);

    SECTION("Wrapping value gives access to raw value")
    {
        auto* expected = INPUT;

        const auto subject = jl::value{jl::from_raw, INPUT};

        REQUIRE(subject.raw() == expected);
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
        const auto wrapped = jl::value{jl::from_raw, INPUT};

        const auto subject = wrapped; // NOLINT(performance-unnecessary-copy-initialization)

        REQUIRE(subject.raw() == expected);
    }

    SECTION("Moving wrapped value keeps underlying raw value")
    {
        auto* expected = INPUT;
        auto wrapped = jl::value{jl::from_raw, INPUT};

        const auto subject = std::move(wrapped);

        REQUIRE(subject.raw() == expected);
    }

    SECTION("Wrapped copy-assignment updates underlying raw value")
    {
        auto* expected = INPUT;
        auto subject = jl::value{jl::from_raw, DUMMY};
        const auto wrapped = jl::value{jl::from_raw, INPUT};

        subject = wrapped; // NOLINT(performance-unnecessary-copy-initialization)

        REQUIRE(subject.raw() == expected);
    }

    SECTION("Wrapped move-assignment updates underlying raw value")
    {
        auto* expected = INPUT;
        auto subject = jl::value{jl::from_raw, DUMMY};
        auto wrapped = jl::value{jl::from_raw, INPUT};

        subject = std::move(wrapped);

        REQUIRE(subject.raw() == expected);
    }

    SECTION("Copying wrapped value roots new value")
    {
        const auto wrapped = jl::value{jl::from_raw, INPUT};

        {
            const auto subject = wrapped; // NOLINT(performance-unnecessary-copy-initialization)
            UNUSED(subject);

            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 2);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
    }

    SECTION("Moving wrapped does not re-root")
    {
        auto wrapped = jl::value{jl::from_raw, INPUT};

        {
            const auto subject = std::move(wrapped);
            UNUSED(subject);

            REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 1);
        }

        REQUIRE(num_rooted() == NUM_BACKGROUND_ROOTED + 0);
    }

    SECTION("Wrapped value copy-assignment frees old and roots new")
    {
        auto subject = jl::value{jl::from_raw, DUMMY};
        const auto wrapped = jl::value{jl::from_raw, INPUT};

        subject = wrapped;
        UNUSED(subject);

        REQUIRE(num_rooted(DUMMY) == 0);
        REQUIRE(num_rooted(INPUT) == 2);
    }

    SECTION("Wrapped value move-assignment frees old and does not re-root")
    {
        auto subject = jl::value{jl::from_raw, DUMMY};

        // create separate scope because `DUMMY` is only destroyed
        // when `other` goes out of scope
        {
            auto wrapped = jl::value{jl::from_raw, INPUT};

            subject = std::move(wrapped);
            UNUSED(subject);
        }

        REQUIRE(num_rooted(DUMMY) == 0);
        REQUIRE(num_rooted(INPUT) == 1);
    }

    SECTION("Unary function invocation with value argument")
    {
        auto* raw = jl_get_function(jl_base_module, "-");
        const auto input = jl::value{jl::from_raw, jl_box_int64(7)};
        const auto expected = "-7";
        const auto subject = jl::value(jl::from_raw, raw);

        const auto result = subject(input);

        REQUIRE(repr(result) == expected);
    }

    SECTION("n-ary function invocation")
    {
        auto* raw = jl_get_function(jl_base_module, "+");
        const auto input_a = jl::value{jl::from_raw, jl_box_int64(7)};
        auto input_b = jl::value{jl::from_raw, jl_box_uint32(7u)};
        const auto input_c = jl::value{jl::from_raw, jl_box_float64(7.f)};
        const auto expected = "21.0";
        const auto subject = jl::value(jl::from_raw, raw);

        const auto result = subject(input_a, std::move(input_b), input_c);

        REQUIRE(repr(result) == expected);
    }
}
