#pragma once

#include "Boxing.hpp"

#include <julia.h>
#include <type_traits>

namespace jl
{

class value;

template<typename ElemT>
class array
{
public:
    template<typename... ElemTs,
             typename CommT = std::common_type_t<ElemTs...>,
             typename std::enable_if_t<std::is_same_v<CommT, ElemT>>* = nullptr>
    array(ElemTs... elems_) noexcept
        : array{impl::get_type<CommT>(), sizeof...(ElemTs)}
    {
        impl::make_arg_vec<ElemT, ElemTs...>::make(_arr, 0, elems_...);
    }

    array(jl_datatype_t* type_, std::size_t size_) noexcept : _size{size_}
    {
        jl_value_t* array_type{jl_apply_array_type(
            reinterpret_cast<jl_value_t*>(type_), dimensions)};
        _arr = static_cast<ElemT*>(std::malloc(_size * sizeof(ElemT)));
        _metadata = jl_ptr_to_array_1d(array_type, _arr, _size, true);
    }

    array(jl_array_t* metadata_) noexcept
        : _arr{static_cast<ElemT*>(jl_array_data(metadata_))}
        , _metadata{metadata_}
        , _size{jl_array_len(metadata_)}
    {
    }

    array(const array&) noexcept = default;
    array(array&&) noexcept = default;

    std::size_t size() const noexcept { return _size; }
    bool empty() const noexcept { return _size > 0; }
    ElemT* data() noexcept { return _arr; }
    jl_array_t* get_boxed_data() noexcept { return _metadata; }
    ElemT* begin() noexcept { return data(); }
    ElemT* end() noexcept { return data() + _size; }
    const ElemT* cbegin() const noexcept { return data(); }
    const ElemT* cend() const noexcept { return data() + _size; }
    ElemT operator[](std::size_t index_) { return data()[index_]; }
    const ElemT operator[](std::size_t index_) const { return data()[index_]; }
    ElemT front() { return *data(); }
    ElemT back() { return data()[_size - 1]; }
    const ElemT front() const { return *data(); }
    const ElemT back() const { return data()[_size - 1]; }

private:
    static constexpr std::size_t dimensions{1};
    ElemT* _arr;
    jl_array_t* _metadata;
    const std::size_t _size;
};

} // namespace jl