#define JULIAPP_DEBUG
#include "Conversions.hpp" // must be included BEFORE julia.hpp
#include "Julia.hpp"

#include <algorithm>

namespace jl
{

boxed_value convert(Vec2 v_)
{
    return jl::call("SVector", v_.x, v_.y);
}

} // namespace jl

void fn(double) {}

int main()
{
    jl::init();

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

    // jl::use("StaticArrays");
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

    jl::array<int> arr{5, 8, 1};
    jl::array<int> reversed_arr{jl::call("reverse", arr)};
    jl::call("println", arr);
    // std::reverse(reversed_arr.begin(), reversed_arr.end());
    // jl::call("println", reversed_arr);

    std::printf("2nd element: %d\n", arr[1]);
    std::printf("Back element: %d\n", arr.back());

    // jl::raise_error("Test");

    // jl::exec_from_file("sample-script.jl");

    // std::printf("%d\n", jl::call("÷", 5, 2).get<int>());

    jl::quit();
}
