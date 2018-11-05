#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

TEST_CASE("Boxing of primitive types")
{
    CHECK(jl::value{5}.get<int>() == 5);
    CHECK(jl::value{3.}.get<float>() == 3.f);

    // WONTFIX: Make this work
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

    jl::eval("mutable struct NoCopy x::Float32 end");
    jl::sync(jl::type<NoCopy>{"NoCopy"});

    auto x = NoCopy{3.14f};
    // clang-format off
    REQUIRE(jl::value{std::move(x)}->x == 3.14f);
    // clang-format on
}

TEST_CASE("Copying variables into values")
{
    auto x = 5;
    auto xv = jl::value{x};

    jl::value<jl::any>{};

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

TEST_CASE("References to mutable/immutable structs")
{
    jl::eval(R"(
      mutable struct Mut x::Int64 end
      struct Immut x::Int64 end
    )");

    struct Mut
    {
        int64_t x;
        Mut& operator=(const Mut&) = default;
    };

    struct Immut
    {
        int64_t x; // should be const in practice
        Immut&
            operator=(const Immut&) = default; // should be delete in practice
    };

    jl::sync(jl::type<Mut>{"Mut"}, jl::type<Immut>{"Immut"});
    auto mut = jl::value<Mut>{{3}};
    auto immut = jl::value<const Immut>{{7}};
    CHECK_THROWS_AS(jl::value<Immut>{{9}}, jl::test::failed_assertion);

    REQUIRE((*mut).x == 3);
    mut->x = 5;
    REQUIRE((*mut).x == 5);
    REQUIRE(mut->x == 5);

    CHECK_NOTHROW(jlpp_static_assert(
        std::is_const_v<std::remove_reference_t<decltype(*immut)>>));
}

TEST_CASE("Primitive value dereferencing")
{
    auto primit = jl::value{12};
    CHECK_NOTHROW(
        jlpp_static_assert(!std::is_lvalue_reference_v<decltype(*primit)>));
}
