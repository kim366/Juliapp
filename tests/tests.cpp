#include <catch2/catch_test_macros.hpp>
#include <julia.hpp>

TEST_CASE("Raw symbol contains the given name")
{
    const auto input = "hello";
    const auto expected = std::string{input};
    const auto symbol = jl::symbol{input};

    const auto result = jl_symbol_name(symbol.raw());

    REQUIRE(result == expected);
}