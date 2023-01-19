#include "boxing.hpp"

namespace jl
{

template<typename T>
T value_cast(value&& val)
{
    return value_cast<T>(val);
}

template<typename T>
T value_cast(const value& val)
{
    return jl::impl::unbox<T>(val.raw());
}

template<typename T>
value::value(const T& val)
    : value{from_raw, impl::box(val)}
{
}

template<typename... Ts>
value value::operator()(Ts&&... args) const
{
    value values[]{std::forward<Ts>(args)...};
    return value{from_raw, jl_call(
        raw(), reinterpret_cast<jl_value_t**>(values), sizeof...(args))};
}

} // namespace jl
