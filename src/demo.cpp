#include "Julia.hpp"

#include <algorithm>

void fn(double) {}

int main()
{

    jl::init();

    struct S
    {
        int64_t x;
        int64_t y;
        float f;
    };

    S* s = jl::exec(R"(
        struct S
          x::Int64
          y::Int64
          f::Float32
        end

        S(16660000, 88235, 3.847)
    )")
               .get<S*>();

    printf("(%ld, %ld, %f)\n", s->x, s->y, s->f);

    jl::exec("S(0)");

    long res;
    try
    {
        res = jl::call("Mod.f", 2.);
        std::printf("%ld\n", res);
    }
    catch (jl::result_type_error&)
    {
        std::puts("Result type error caught.");
    }

    try
    {
        jl::exec("nonexistent_function()");
    }
    catch (jl::language_error&)
    {
        std::puts("Language error caught.");
    }

    try
    {
        jl::call("nonexistent_function");
    }
    catch (jl::language_error&)
    {
        std::puts("Language error caught.");
    }

    jl::array<int> arr{5, 8, 1};
    jl::array<int> reversed_arr{jl::call("reverse", arr)};
    jl::call("println", arr);
    std::reverse(reversed_arr.begin(), reversed_arr.end());
    jl::call("println", reversed_arr);

    std::printf("2nd element: %d\n", arr[1]);
    std::printf("Back element: %d\n", arr.back());

    fn(jl::call("Mod.f", 2.));

    // jl::raise_error("Test");

    jl::exec_from_file("sample-script.jl");

    std::printf("%d\n", jl::call("÷", 5, 2).get<int>());

    jl::quit();
}
