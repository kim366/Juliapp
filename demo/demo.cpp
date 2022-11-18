#define JULIAPP_DEBUG
#include "julia.hpp"

int main()
{
    using namespace jl::literals;

    jl::eval("x::Int = 17");
    const auto x = jl::value_cast<int>(*jl::main["x"]);
    const auto y = jl::value_cast<int>("x"_jlv);
    std::printf("%d %d\n", x, y);
}
