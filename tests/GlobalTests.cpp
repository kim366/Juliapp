#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

using namespace jl::literals;

TEST_CASE("Assigning new value to global")
{
    jl::eval("to_override = 3");
    auto to_override = "to_override"_jlg;
    REQUIRE((int)to_override == 3);

    to_override = 5;
    REQUIRE((int)to_override == 5);
}