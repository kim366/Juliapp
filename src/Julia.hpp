#pragma once

#include "Boxing.hpp"
#include "Helpers.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <julia.h>
#include <sstream>
#include <string>

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

class value
{
public:
    value(jl_value_t* boxed_value_) noexcept : _boxed_value{boxed_value_} {}

    value() noexcept = default;

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

    template<typename ElemT>
    explicit operator array<ElemT>() noexcept
    {
        return reinterpret_cast<jl_array_t*>(_boxed_value);
    }

private:
    jl_value_t* _boxed_value;
};

inline value eval(const char* src_str_)
{
    jl_value_t* res{jl_eval_string(src_str_)};
    impl::check_err();
    return res;
}

inline value exec_from_file(const char* file_name_)
{
    std::ifstream file{file_name_};
    if (!file.is_open())
        throw load_error{std::string{"Could not load script "} + file_name_};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return eval(buffer.str().c_str());
}

template<typename... ArgTs>
value call(const char* fn_name_, ArgTs... args_)
{
    std::array<jl_value_t*, sizeof...(args_)> boxed_args{impl::box(args_)...};

    jl_value_t* func{jl_eval_string(fn_name_)};
    impl::check_err();
    jl_value_t* res{jl_call(func, boxed_args.data(), boxed_args.size())};
    impl::check_err();
    return res;
}

inline void init()
{
    jl_init();
}

inline void quit(int code_ = 0)
{
    jl_atexit_hook(code_);
}

inline void raise_error(const char* content_)
{
    jl_error(content_);
}

template<typename... ArgTs>
void raise_error(const char* content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

inline value eval(const std::string& src_str_)
{
    return eval(src_str_.c_str());
}

inline value exec_from_file(const std::string& file_name_)
{
    return exec_from_file(file_name_.c_str());
}

template<typename... ArgTs>
value call(const std::string& fn_name_, ArgTs... args_)
{
    return call(fn_name_.c_str(), args_...);
}

template<typename... ArgTs>
void raise_error(const std::string& content_, ArgTs... args_)
{
    raise_error(content_.c_str(), args_...);
}

} // namespace jl
