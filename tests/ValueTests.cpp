#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

TEST_CASE("Boxing of primitive types")
{
    CHECK(jl::value{5}.get<int>() == 5);
    CHECK(jl::value{3.}.get<float>() == 3.f);

    // TODO: Make this work
    // CHECK(jl::value{2.}.get<int>() == 2);
}