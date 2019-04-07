#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

TEST_CASE("Calling Functions with free function")
{
    jl::eval("f(x) = 3x; g(x::Int32) = 2x");
    REQUIRE(jl::call("f", 5).get<int>() == 15);
    REQUIRE(jl::call("f", 3.).get<double>() == 9.);
    REQUIRE(jl::call("f", 3.).get<float>() == 9.f);
    CHECK_THROWS_AS(jl::call("g", 5ULL), jl::language_error);
    CHECK_THROWS_AS(jl::call("h", 13.), jl::language_error);
}