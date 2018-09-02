#pragma once

#include "Boxing.hpp"

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
        std::initializer_list<CommT> elem_list{elems_...};
        ElemT* _arr_data{data()};
        std::copy(elem_list.begin(), elem_list.end(), _arr_data);
    }

    array(jl_datatype_t* type_, std::size_t size_) noexcept : _size{size_}
    {
        jl_value_t* array_type{jl_apply_array_type(
            reinterpret_cast<jl_value_t*>(type_), dimensions)};

        _arr = jl_alloc_array_1d(array_type, size_);
    }

    array(jl_array_t* arr_) noexcept : _arr{arr_}, _size{arr_->length} {}

    array(const array&) noexcept = default;
    array(array&&) noexcept = default;

    std::size_t size() const noexcept { return _size; }
    bool empty() const noexcept { return _size > 0; }
    ElemT* data() noexcept
    {
        return reinterpret_cast<ElemT*>(jl_array_data(_arr));
    }
    jl_array_t* get_boxed_data() noexcept { return _arr; }
    ElemT* begin() noexcept { return data(); }
    ElemT* end() noexcept { return data() + _size; }
    const ElemT* cbegin() const noexcept { return data(); }
    const ElemT* cend() const noexcept { return data() + _size; }
    ElemT& operator[](std::size_t index_) { return data()[index_]; }
    const ElemT& operator[](std::size_t index_) const { return data()[index_]; }
    ElemT& front() { return *data(); }
    ElemT& back() { return data()[_size - 1]; }
    const ElemT& front() const { return *data(); }
    const ElemT& back() const { return data()[_size - 1]; }

private:
    static constexpr std::size_t dimensions{1};
    jl_array_t* _arr;
    const std::size_t _size;
};

} // namespace jl
