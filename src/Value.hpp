#pragma once

#include "Array.hpp"
#include "Function.hpp"
#include "Init.hpp"

#include <julia_gcext.h>

namespace jl
{

namespace impl
{

class common_value
{
public:
    common_value(jl_value_t* boxed_value_) noexcept : _boxed_value{boxed_value_}
    {
        root_value(_boxed_value);
    }

    common_value() noexcept : common_value{nullptr} {}

    ~common_value() { release_value(_boxed_value); }

protected:
    jl_value_t* c_val() { return _boxed_value; }

    bool operator==(const common_value& rhs) const
    {
        return static_cast<bool>(jl_egal(_boxed_value, rhs._boxed_value));
    }

    bool operator!=(const common_value& rhs) const { return !(rhs == *this); }

    template<typename TargT,
             std::enable_if_t<std::is_fundamental<TargT>{}
                              || impl::is_array<TargT>{}>* = nullptr>
    TargT get()
    {
        if constexpr (std::is_integral_v<TargT>)
            return static_cast<TargT>(impl::unbox<long>(_boxed_value));
        else if constexpr (std::is_floating_point_v<TargT>)
            return static_cast<TargT>(impl::unbox<double>(_boxed_value));
        else
            return *this;
    }

    template<typename TargT,
             std::enable_if_t<!std::is_fundamental<TargT>{}>* = nullptr>
    TargT get() noexcept
    {
        if constexpr (std::is_pointer_v<TargT>)
            return reinterpret_cast<TargT>(_boxed_value);
        else
            return *reinterpret_cast<std::decay_t<TargT>*>(_boxed_value);
    }

    jl_value_t* _boxed_value;
};

} // namespace impl

template<typename ValT>
struct value : private impl::common_value
{
    using impl::common_value::common_value;
    //    value() = default;

    value(ValT&& obj_) : common_value{impl::box(std::forward<ValT>(obj_))} {}

    value(const ValT& obj_) : common_value{impl::box(obj_)} {}

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
};

struct runtime_value : private impl::common_value
{
    using common_value::common_value;
    using common_value::get;
    using common_value::operator!=;
    using common_value::operator==;

    runtime_value() = default;

    template<typename TargT,
             typename = std::enable_if_t<std::is_fundamental<TargT>{}>>
    operator TargT()
    {
        return impl::unbox<TargT>(_boxed_value);
    }

    template<typename TargT,
             std::enable_if_t<std::is_class_v<std::decay_t<TargT>>>* = nullptr>
    operator TargT()
    {
        return get<TargT>();
    }

    template<typename ElemT>
    explicit operator array<ElemT>() noexcept
    {
        return reinterpret_cast<jl_array_t*>(_boxed_value);
    }

    function get_function() { return function{c_val()}; }
};

} // namespace jl