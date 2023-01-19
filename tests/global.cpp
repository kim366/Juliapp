#include "julia.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

TEST_CASE("Global")
{
    SECTION("Raw binding can be converted to global")
    {
        auto* raw = reinterpret_cast<jl_binding_t*>(0x123456);
        auto* expected = raw;
        const auto subject = jl::global{jl::from_raw, raw};

        REQUIRE(subject.raw() == expected);
    }

    SECTION("Value can be read from global")
    {
        jl::eval("x = 7");
        const auto expected = 7;
        const auto subject = jl::main["x"];

        const auto result = *subject;

        REQUIRE(jl::value_cast<jl::int_t>(result) == expected);
    }

    SECTION("Value can be written to global")
    {
        jl::eval("y = 7");
        const auto input = jl::int_t{15};
        const auto expected = input;
        auto subject = jl::main["y"];
        const auto wrapped = jl::value{input};

        const auto& result = subject.assign(wrapped);

        REQUIRE(&result == &wrapped);
        REQUIRE(jl::value_cast<jl::int_t>(jl::eval("y")) == expected);
    }

    SECTION("Value is adequately converted on write")
    {
        jl::eval("a::Int = 7");
        const auto input = 15.f;
        const auto expected = jl::int_t{15};
        auto subject = jl::main["a"];
        const auto wrapped = jl::value{input};

        const auto& result = subject.assign(wrapped);

        REQUIRE(&result == &wrapped);
        REQUIRE(jl::value_cast<jl::int_t>(jl::eval("a")) == expected);
    }

    SECTION("Constant cannot be written to global")
    {
        jl::eval("const z = 7");
        const auto input = jl::int_t{15};
        auto subject = jl::main["z"];
        const auto wrapped = jl::value{input};

        REQUIRE_THROWS_MATCHES(subject.assign(wrapped), std::logic_error, Catch::Matchers::Message("Cannot assign to constant z"));
    }
}