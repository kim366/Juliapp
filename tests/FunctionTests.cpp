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

    auto f = jl::call("returning_function").get_function();

    CHECK(f(2).get<int>() == 6);
}