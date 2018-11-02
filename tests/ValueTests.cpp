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

TEST_CASE("Moving variables into values")
{

    struct NoCopy
    {
        NoCopy(const NoCopy&) = delete;
        NoCopy(NoCopy&&) = default;

        NoCopy& operator=(const NoCopy&) = delete;
        NoCopy& operator=(NoCopy&&) = default;

        float x;
    };

    jl::sync(jl::type<NoCopy>{"struct NoCopy x::Float32 end"});
    auto x = NoCopy{3.14f};

    REQUIRE(jl::typed_value { std::move(x) }->x == 3.14f);
}

TEST_CASE("Copying variables into values")
{
    auto x = 5;
    auto xv = jl::typed_value{x};

    REQUIRE(x == 5);
    REQUIRE(*xv == 5);
}

TEST_CASE("Literals")
{
    using namespace jl::literals;
    jl::eval("module SomeMod nested(x) = 3x end; free(x) = 2x");
    REQUIRE("SomeMod"_jlm == jl::module{"SomeMod"});
    REQUIRE("free"_jlf == jl::function{"free"});
}
