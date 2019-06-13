#define JULIAPP_DEBUG
#include "Julia.hpp"

#include <iostream>

int main()
{

    jl::eval(R"(
        f(x) = (println("{",x,"}"); 2x)
        )");

    jl::function f("f");
    std::cout << ">>> " << int(f(1337)) << std::endl;

    auto fp = reinterpret_cast<int (*)(int)>(
        jl::eval("fp = @cfunction(f, Int, (Int,))").get<void*>());

    std::cout << ">>> " << fp(1234) << std::endl;
}
