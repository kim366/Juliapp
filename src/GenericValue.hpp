#pragma once

#include <Helpers.hpp>
#include <julia.h>

namespace jl
{

class function;
struct module;

class generic_value
{
public:
    generic_value(jl_value_t* boxed_value_) noexcept;

    generic_value() noexcept;
    generic_value(const generic_value& other);
    generic_value(generic_value&& other);

    ~generic_value();

    jl_value_t* c_val() const;

    bool operator==(const generic_value& rhs) const;

    bool operator!=(const generic_value& rhs) const;

    template<typename TargT,
             std::enable_if_t<std::is_fundamental<TargT>{}
                              || impl::is_array<TargT>{}>* = nullptr>
    TargT get();

    template<typename TargT,
             std::enable_if_t<!std::is_fundamental<TargT>{}>* = nullptr>
    TargT get() noexcept;

    template<typename TargT,
             typename = std::enable_if_t<std::is_fundamental<TargT>{}>>
    operator TargT();

    template<typename TargT,
             std::enable_if_t<std::is_class_v<std::decay_t<TargT>>>* = nullptr>
    operator TargT();

    template<typename ElemT>
    explicit operator array<ElemT>() noexcept;

    generic_value& operator=(generic_value other_);

    generic_value generic() const&;
    generic_value generic() &&;

    function as_function();
    module as_module();

    template<typename T>
    bool typeis();

    friend void swap(generic_value& first, generic_value& second);

protected:
    jl_value_t* _boxed_value;
};

} // namespace jl