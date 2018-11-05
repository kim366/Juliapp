#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

TEST_CASE("Calling Functions with free function")
{
    jl::eval("f(x) = 3x; g(x::Int32) = 2x");
    REQUIRE(jl::call("f", 5).get<int>() == 15);
    try
    {
        (jl::call("g", 5ULL));
    }
    catch (std::exception& e)
    {
        puts(e.what());
    };
}