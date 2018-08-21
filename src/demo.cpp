#include <Julia.hpp>

int main()
{

    jl_init();

    jl(R"(
        module Mod
          const a = 6.
          f(x) = a * x^3
        end    
    )");

    long res;
    try
    {
        res = jl("Mod.f", 2.);
        std::printf("%ld\n", res);
    }
    catch (const std::logic_error&)
    {
        std::puts("Logic error caught.");
    }

    std::printf("%ld\n", jl("÷", 5, 2).get<long>());

    jl_atexit_hook(0);
}
