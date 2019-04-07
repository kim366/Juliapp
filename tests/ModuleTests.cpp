#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

TEST_CASE("Function returning Module")
{
    jl::eval(
        "module ReturnedModule f() = 3 end; returning_module() = ReturnedModule");
    auto m = jl::call("returning_module").get_module();
    auto f = jl::function{m, "f"};
    CHECK(f().get<int>() == 3);
}