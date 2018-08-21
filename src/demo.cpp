#include <Julia.hpp>

int main()
{

    jl::init();

    jl::exec(R"(
        module Mod
          const a = 6.
          f(x) = a * x^3
        end    
    )");

    long res;
    try
    {
        res = jl::call("Mod.f", 2.);
        std::printf("%ld\n", res);
    }
    catch (const std::logic_error&)
    {
        std::puts("Logic error caught.");
    }

    try
    {
        jl::exec("nonexistent_function()");
    }
    catch (jl::error)
    {
        std::puts("Julia error caught.");
    }

    try
    {
        jl::call("nonexistent_function", 3);
    }
    catch (jl::error)
    {
        std::puts("Julia error caught.");
    }

    // jl::raise_error("Test");

    jl::exec_from_file("sample-script.jl");

    std::printf("%d\n", jl::call("÷", 5, 2).get<int>());

    jl::quit();
}
