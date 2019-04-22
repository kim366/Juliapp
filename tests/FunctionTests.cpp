#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

using namespace jl::literals;

TEST_CASE("Function objects")
{
    jl::eval("thefunction(x)::Int32 = 4x");
    auto f = "thefunction"_jlf;

    CHECK_THROWS_AS(f(), jl::language_error);
    long long res = f(3);
    CHECK(res == 12);
}

TEST_CASE("Function returning function")
{
    jl::eval("returning_function() = x -> 3x");

    auto f = "returning_function"_jlf().as_function();

    CHECK(f(2).get<int>() == 6);
}

TEST_CASE("Function taking data type value")
{
    auto integer = "Core"_jlm["Int64"];

    CHECK("sizeof"_jlf(integer.value()).get<jl::int_t>() == 8);
}

TEST_CASE("Function taking data type binding")
{
    auto integer = "Core"_jlm["Int64"];
    CHECK("sizeof"_jlf(integer.value()).get<jl::int_t>() == 8);
}
