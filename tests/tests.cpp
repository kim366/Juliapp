#include <catch2/catch_test_macros.hpp>

TEST_CASE("Catch2 works")
{
    const auto input_a = 3;
    const auto input_b = 7;
    const auto expected = 10;

    const auto result = input_a + input_b;

    REQUIRE(result == expected);
}