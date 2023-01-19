#include "julia.hpp"
#include "utility.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Evaluation")
{
    const auto* input = "1 + 17";
    const auto expected = 18;

    const auto result = jl::eval(input);

    REQUIRE(jl::value_cast<jl::int_t>(result) == expected);
}

TEST_CASE("Rooted values survive GC collecitons")
{
    const auto subject = jl::eval("[1, 2, 3]");

    jl_gc_collect(JL_GC_FULL);

    REQUIRE(repr(subject) == "[1, 2, 3]");
}
