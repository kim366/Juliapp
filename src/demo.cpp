#include <Julia.hpp>

int main()
{

    jl_init();

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

    std::printf("%d\n", jl::call("÷", 5, 2).get<int>());

    jl_atexit_hook(0);
}
