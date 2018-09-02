#pragma once

#include <type_traits>

namespace jl
{

template<typename T>
class array;

namespace impl
{

template<typename>
struct is_array : std::false_type
{
};

template<typename ElemT>
struct is_array<jl::array<ElemT>> : std::true_type
{
};

template<typename T>
class is_convertible
{
    template<typename C, typename = decltype(convert(std::declval<C>()))>
    static std::true_type test(int);

    template<typename>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

} // namespace impl

template<typename T>
concept bool ConvertibleToJlValue = impl::is_convertible<T>{};

} // namespace jl
