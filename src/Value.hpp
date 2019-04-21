#pragma once

#include "GenericValue.hpp"

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

    template<typename std::enable_if_t<std::is_fundamental_v<ValT>>* = nullptr>
    ValT operator*();

    template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>* = nullptr>
    ValT& operator*();

    template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>* = nullptr>
    const ValT& operator*() const;

    ValT* operator->();
};

} // namespace jl