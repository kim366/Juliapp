#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

using namespace jl::literals;

TEST_CASE("Assigning new data to global")
{
    jl::eval("to_override = 3");
    auto to_override = "to_override"_jlg;
    REQUIRE((int)to_override == 3);

    to_override = 5;
    REQUIRE((int)to_override == 5);
}

TEST_CASE("Assigning new value to global")
{
    jl::value v = 5;
    jl::eval("to_override_with_value = 3");
    auto to_override = "to_override_with_value"_jlg;
    REQUIRE((int)to_override == 3);

    to_override = v;
    REQUIRE((int)to_override == 5);
}

TEST_CASE("Assigning new generic value to global")
{
    auto v = jl::value{5}.generic();
    jl::eval("to_override_with_value = 3");
    auto to_override = "to_override_with_value"_jlg;
    REQUIRE((int)to_override == 3);

    to_override = v;
    REQUIRE((int)to_override == 5);
}