#include "catch.hpp"

#define IMPL_JLPP_UNIT_TESTING
#include <Julia.hpp>

using namespace jl::literals;

TEST_CASE("Calling Functions with free function")
{
    jl::eval("f(x) = 3x; g(x::Int32) = 2x");
    REQUIRE("f"_jlf(5).get<int>() == 15);
    REQUIRE("f"_jlf(3.).get<double>() == 9.);
    REQUIRE("f"_jlf(3.).get<float>() == 9.f);
    CHECK_THROWS_AS("g"_jlf(5ULL), jl::language_error);
    CHECK_THROWS_AS("h"_jlf(13.), jl::language_error);
}