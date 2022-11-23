#define JULIAPP_DEBUG
#include "julia.hpp"

int main()
{
    using namespace jl::literals;

    jl::eval("module M; export fn, x; x = 22; fn(x) = println(\"Hello world $x\") end; import .M: fn, x");
    auto x = jl::main["x"];

    auto a = "fn"_jlf;
    x.set(9999);
   a(7);
   jl::eval("println(x)");
}
