#define JULIAPP_DEBUG
#include "Julia.hpp"
#include "Sync.hpp"

#include <algorithm>

void fn(double) {}

struct Vec2
{
    Vec2(float x, float y) : x{x}, y{y} {}
    float x, y;
};

int main()
{
    using namespace jl::literals;

    //    jl::use("StaticArrays");

    jl::eval(R"(
      module Mod
        mutable struct Vec2
          x::Float32
          y::Float32
        end

        f(x) = println(x)
      end
     )");
    jl::sync(jl::type<Vec2>{"NTuple{2, Float32}"});

    jl::function f{"Mod"_jlm, "f"};
    f(1337);

    auto jl_vec{jl::make_value<Vec2>(7.f, 1.f)};
    static_assert(std::is_same_v<decltype(jl_vec), jl::value<Vec2>>);
    std::printf("From C: %f\n", jl_vec->x);
    jl_vec->x = 7.12345f;
    std::printf("%f\n", jl_vec.get<Vec2>().x);

    // struct S
    // {
    //     struct
    //     {
    //         float x, y;
    //     } v;
    //     // or float v[2];

    //     std::int64_t y;
    //     float f;
    // };

    // S& s = jl::eval(R"(
    //     mutable struct S
    //       x::NTuple{2, Float32}
    //       y::Int64
    //       f::Float32
    //     end

    //     s = S((1234, 5678), 88235, 3.847)
    // )")
    //            .get<S&>();

    // jl::use("LinearAlgebra");

    // Vec2 v;
    // v = jl::call("+", Vec2{1.334f, 5.67f}, Vec2{6.254f, 9.127f});
    // printf("v = (%f, %f)\n", v.x, v.y);

    // printf("((%f, %f) %ld, %f)\n", s.v.x, s.v.y, s.y, s.f);
    // s.v.x = 5;
    // jl::call("println", s);

    // try
    // {
    //     jl::eval("nonexistent_function()");
    // }
    // catch (jl::language_error&)
    // {
    //     std::puts("Language error caught.");
    // }

    // try
    // {
    //     jl::call("nonexistent_function");
    // }
    // catch (jl::language_error&)
    // {
    //     std::puts("Language error caught.");
    // }

    //    jl::array<int> arr{5, 8, 1};
    //    jl::array<int> reversed_arr{jl::call("reverse", arr)};
    //    jl::call("println", arr);
    jl::call("println", jl_vec);
    jl::call("println", Vec2{2, 3});
    // jl::call("println", jl_vec);
    /* TODO: Fix this call; if called instead of above a jl:: call, jl_vec gets
     * garbage collected. */
    //    std::reverse(reversed_arr.begin(), reversed_arr.end());
    //    jl::call("println", reversed_arr);
    //    jl::call("println ∘ collect ∘ UnitRange", 1000, 1005);

    //    std::printf("2nd element: %d\n", arr[1]);
    //    std::printf("Back element: %d\n", arr.back());

    // jl::raise_error("Test");

    // jl::exec_from_file("sample-script.jl");

    // std::printf("%d\n", jl::call("÷", 5, 2).get<int>());
}
