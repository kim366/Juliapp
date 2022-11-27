#define JULIAPP_DEBUG
#include "julia.hpp"

using namespace jl::literals;

struct MyStruct;

template<>
struct jl::analogous<MyStruct>
{
    jl::value operator()() const
    {
        static auto type = "MyStruct"_jlv;
        return type;
    }
};

struct MyStruct
{
    jl::int_t x;
    double y;
};

int main()
{
    jl::eval("struct MyStruct; x::Int; y::Float64 end");
    jl::eval("module M; export fn, x; x = 22; fn(x) = println(\"Hello world $x\") end; import .M: fn, x");
    auto x = jl::main["x"];
    auto s = jl::eval("MyStruct(3, 5.5)");
    auto sval = jl::value_cast<MyStruct>(s);

    auto a = "fn"_jlv;
//    x.set(9999);
   a(7);
   jl::eval("println(x)");
}
