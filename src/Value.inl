#pragma once

#include <Init.hpp>
#include <Value.hpp>

namespace jl
{

template<typename ValT>
template<typename std::enable_if_t<std::is_fundamental_v<ValT>>*>
ValT value<ValT>::operator*()
{
    return get<ValT>();
}

template<typename ValT>
template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>*>
ValT& value<ValT>::operator*()
{
    return impl::unbox<ValT&>(_boxed_value);
}

template<typename ValT>
template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>*>
const ValT& value<ValT>::operator*() const
{
    return impl::unbox<ValT&>(_boxed_value);
}

template<typename ValT>
ValT* value<ValT>::operator->()
{
    return &**this;
}

} // namespace jl