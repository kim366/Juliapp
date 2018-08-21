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

    double res{jl("Mod.f", 2.)};
    std::printf("%f\n", res);

    jl_atexit_hook(0);
}
