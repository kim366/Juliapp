#define JULIAPP_DEBUG
#include "Julia.hpp"

#include <functional>
#include <iostream>

namespace jl
{

namespace impl
{

template<typename T>
inline std::string typename_str()
{
    return std::string(jl_symbol_name(jl::impl::get_type<T>()->name->name));
}

template<typename... Ts>
struct _typestuple;

template<typename T, typename... Ts>
struct _typestuple<T, Ts...>
{
    static inline std::string str()
    {
        return typename_str<T>() + "," + _typestuple<Ts...>::str();
    }
};

template<>
struct _typestuple<>
{
    static inline std::string str() { return std::string(); }
};

template<typename... Ts>
struct typestuple
{
    inline static std::string str()
    {
        return std::string("(") + _typestuple<Ts...>::str() + std::string(")");
    }
};

template<typename F>
struct _cfunction;

template<typename R, typename... Args>
struct _cfunction<R(Args...)>
{
    using returntype = R;
    using funcpointertype = R (*)(Args...);
    using argtypestuple = std::tuple<Args...>;
    inline static std::string returntype_str() { return typename_str<R>(); }
    inline static std::string argtypestuple_str()
    {
        return typestuple<Args...>::str();
    }
};

} // namespace impl

template<typename Signature>
std::function<Signature> cfunction(const std::string& name)
{
    using funcpointertype =
        typename impl::_cfunction<Signature>::funcpointertype;
    auto returntype_str = impl::_cfunction<Signature>::returntype_str();
    auto argtypestuple_str = impl::_cfunction<Signature>::argtypestuple_str();
    std::string jl_src = std::string("@cfunction(") + //
                         name + ", " +                //
                         returntype_str + ", " +      //
                         argtypestuple_str + ")";
    funcpointertype fp = funcpointertype(jl::eval(jl_src).get<void*>());
    return std::function<Signature>(fp);
}

} // namespace jl

int main()
{
    jl::eval(R"(
                    function f(x)
                        println("{",x,"}")
                        return x*x
                    end
             )");

    auto f = jl::function("f");
    std::cout << ">>> " << double(f(123.4)) << std::endl;

    auto cf_int = jl::cfunction<int(int)>("f");
    std::cout << ">>> " << cf_int(123) << std::endl;

    auto cf_double = jl::cfunction<double(double)>("f");
    std::cout << ">>> " << cf_double(123.4) << std::endl;
}
