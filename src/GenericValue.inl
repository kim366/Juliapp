#pragma once

#include "GenericValue.hpp"

#include <Boxing.hpp>
#include <Helpers.hpp>
#include <Init.hpp>

namespace jl
{

inline generic_value::generic_value(jl_value_t* boxed_value_) noexcept
    : _boxed_value{boxed_value_}
{
    impl::root_value(_boxed_value);
}

inline generic_value::generic_value() noexcept : generic_value{nullptr} {}

inline generic_value::generic_value(const generic_value& other)
    : generic_value{other.c_val()}
{
}

inline generic_value::generic_value(generic_value&& other) : generic_value{}
{
    swap(*this, other);
}

inline generic_value::~generic_value()
{
    impl::release_value(_boxed_value);
}

inline jl_value_t* generic_value::c_val() const
{
    return _boxed_value;
}

inline bool generic_value::operator==(const generic_value& rhs) const
{
    return static_cast<bool>(jl_egal(_boxed_value, rhs._boxed_value));
}

inline bool generic_value::operator!=(const generic_value& rhs) const
{
    return !(rhs == *this);
}

template<
    typename TargT,
    std::enable_if_t<std::is_fundamental<TargT>{} || impl::is_array<TargT>{}>*>
TargT generic_value::get()
{
    if constexpr (std::is_integral_v<TargT>)
        return static_cast<TargT>(impl::unbox<long>(_boxed_value));
    else if constexpr (std::is_floating_point_v<TargT>)
        return static_cast<TargT>(impl::unbox<double>(_boxed_value));
    else
        return *this;
}

template<typename TargT, std::enable_if_t<!std::is_fundamental<TargT>{}>*>
TargT generic_value::get() noexcept
{
    if constexpr (std::is_pointer_v<TargT>)
        if constexpr (std::is_same_v<TargT, void*>)
            return impl::unbox<TargT>(_boxed_value);
        else
            return reinterpret_cast<TargT>(_boxed_value);
    else
        return *reinterpret_cast<std::decay_t<TargT>*>(_boxed_value);
}

template<typename TargT, typename>
generic_value::operator TargT()
{
    return impl::unbox<TargT>(_boxed_value);
}

template<typename TargT,
         std::enable_if_t<std::is_class_v<std::decay_t<TargT>>>*>
generic_value::operator TargT()
{
    return get<TargT>();
}

template<typename ElemT>
inline generic_value::operator array<ElemT>() noexcept
{
    return reinterpret_cast<jl_array_t*>(_boxed_value);
}

inline generic_value generic_value::generic() const&
{
    return generic_value{*this};
}

inline generic_value generic_value::generic() &&
{
    return generic_value{std::move(*this)};
}

template<typename T>
bool generic_value::typeis()
{
    return jl_typeis(_boxed_value, impl::get_type<T>());
}

inline generic_value& generic_value::operator=(generic_value other_)
{
    swap(*this, other_);
    return *this;
}

inline void swap(generic_value& first, generic_value& second)
{
    using std::swap;
    swap(first._boxed_value, second._boxed_value);
}

} // namespace jl
