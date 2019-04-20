#include <Boxing.hpp>
#include <Helpers.hpp>
#include <Init.hpp>

namespace jl
{

struct function;
struct module;

class generic_value
{
public:
    generic_value(jl_value_t* boxed_value_) noexcept
        : _boxed_value{boxed_value_}
    {
        impl::root_value(_boxed_value);
    }

    generic_value() noexcept : generic_value{nullptr} {}
    generic_value(const generic_value& other) : generic_value{other.c_val()} {}

    ~generic_value() { impl::release_value(_boxed_value); }

    jl_value_t* c_val() const { return _boxed_value; }

    bool operator==(const generic_value& rhs) const
    {
        return static_cast<bool>(jl_egal(_boxed_value, rhs._boxed_value));
    }

    bool operator!=(const generic_value& rhs) const { return !(rhs == *this); }

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

    function as_function();
    module as_module();

protected:
    jl_value_t* _boxed_value;
};

} // namespace jl