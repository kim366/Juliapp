#define JULIAPP_DEBUG
#include "julia.hpp"

int main()
{
    using namespace jl::literals;

    jl::eval("x::Int = 17; fn() = println(\"Hello world\")");
    const auto x = jl::main["x"];
    const auto y = *jl::main["x"];

    auto a = jl::function{jl::value::from_raw(jl_get_binding(jl_main_module, jl_symbol("fn"))->value)};

    jl::eval("x = 20");

   a();

   std::printf("%d %d\n", jl::value_cast<int>(*x), jl::value_cast<int>(y));
}
