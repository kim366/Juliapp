#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

TEST_CASE("Function returning Module")
{
    jl::eval(
        "module ReturnedModule f() = 3 end; returning_module() = ReturnedModule");
    auto m = jl::call("returning_module").as_module();
    jl::module m2 = jl::call("returning_module");
    auto f = jl::function{m, "f"};
    auto f2 = jl::function{m2, "f"};
    CHECK(f().get<int>() == 3);
    CHECK(f2().get<int>() == 3);
}

TEST_CASE("Module subscript operator")
{
    using namespace jl::literals;

    jl::eval("module ModuleForSubscript x = 3; f(a) = a end");

    auto mod = "ModuleForSubscript"_jlm;

    jl::global x = mod["x"];
    int xint = x;
    REQUIRE(xint == 3);
}