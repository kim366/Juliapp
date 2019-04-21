#include "catch.hpp"

#define JLPP_IMPL_UNIT_TESTING
#include <Julia.hpp>

using namespace jl::literals;

struct NoCopy;
JLPP_SYNC(NoCopy, "NoCopy"_jlg);
struct NoCopy
{
    NoCopy(const NoCopy&) = delete;
    NoCopy(NoCopy&&) = default;

    NoCopy& operator=(const NoCopy&) = delete;
    NoCopy& operator=(NoCopy&&) = default;

    float x;
};

struct Mut;
JLPP_SYNC(Mut, "Mut"_jlg);
struct Mut
{
    int64_t x;
    Mut& operator=(const Mut&) = default;
};

struct Immut;
JLPP_SYNC(Immut, "Immut"_jlg);
struct Immut
{
    const int64_t x;
    Immut& operator=(const Immut&) = delete;
};

TEST_CASE("Boxing of primitive types")
{
    CHECK(*jl::value{5} == 5);
    CHECK(*jl::value{3.} == 3.f);

    // WONTFIX: Make this work
    // CHECK(jl::value{2.}.get<int>() == 2);
}

TEST_CASE("Moving variables into values")
{
    jl::eval("mutable struct NoCopy x::Float32 end");

    auto x = NoCopy{3.14f};
    // clang-format off
    REQUIRE(jl::value{std::move(x)}->x == 3.14f);
    // clang-format on
}

TEST_CASE("Copying variables into values")
{
    auto x = 5;
    auto xv = jl::value{x};

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

    auto mut = jl::value<Mut>{{3}};
    auto immut = jl::value<Immut>{{7}};

    REQUIRE((*mut).x == 3);
    mut->x = 5;
    REQUIRE((*mut).x == 5);
    REQUIRE(mut->x == 5);
}

TEST_CASE("Primitive value dereferencing")
{
    auto primit = jl::value{12};
    CHECK_NOTHROW(
        jlpp_static_assert(!std::is_lvalue_reference_v<decltype(*primit)>));
}
