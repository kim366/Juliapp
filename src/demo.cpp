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

    int res;
    try
    {
        res = jl("Mod.f", 2.);
        std::printf("%d\n", res);
    }
    catch (const std::logic_error&)
    {
        std::puts("Logic error caught.");
    }

    jl_atexit_hook(0);
}
