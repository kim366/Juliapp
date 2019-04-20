#pragma once

#include "Array.hpp"
#include "GenericValue.hpp"
#include "Init.hpp"

#include <julia_gcext.h>

namespace jl
{

class function;
class module;

namespace impl
{

} // namespace impl

template<typename ValT>
struct value : public generic_value
{
    value(ValT&& obj_) : generic_value{impl::box(std::forward<ValT>(obj_))} {}
    value(const ValT& obj_) : generic_value{impl::box(obj_)} {}
    value(const value&) = default;

    template<typename std::enable_if_t<std::is_fundamental_v<ValT>>* = nullptr>
    ValT operator*()
    {
        return get<ValT>();
    }

    template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>* = nullptr>
    ValT& operator*()
    {
        return impl::unbox<ValT&>(_boxed_value);
    }

    template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>* = nullptr>
    const ValT& operator*() const
    {
        return impl::unbox<ValT&>(_boxed_value);
    }

    ValT* operator->() { return &**this; }

    generic_value generic() const;
};

} // namespace jl