#pragma once

#include "Boxing.hpp"
#include "Function.hpp"
#include "GenericString.hpp"
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

class value
{
public:
    value(jl_value_t* boxed_value_) noexcept : _boxed_value{boxed_value_} {}

    value() noexcept = default;

    template<typename T>
    value(T&& obj_) : _boxed_value{box(obj_)}
    {
    }

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
        if constexpr (std::is_same_v<TargT, jl_value_t*>)
            return static_cast<jl_value_t*>(*this);
        else if constexpr (std::is_pointer_v<TargT>)
            return reinterpret_cast<TargT>(_boxed_value);
        else
            return *reinterpret_cast<std::decay_t<TargT>*>(_boxed_value);
    }

    template<
        typename TargT,
        typename = std::enable_if_t<std::is_fundamental<TargT>{}
                                    && !std::is_same_v<TargT, jl_value_t*>>>
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

    operator jl_value_t*() { return _boxed_value; }

    template<typename ElemT>
    explicit operator array<ElemT>() noexcept
    {
        return reinterpret_cast<jl_array_t*>(_boxed_value);
    }

private:
    jl_value_t* _boxed_value;
};

template<typename ValT, typename... ArgTs>
value make_value(ArgTs&&... args_)
{
    jl_datatype_t* found{impl::find_synced_jl_type<ValT>()};
    assert(found && "Requested type not synced");
    jl_value_t* val{jl_new_struct_uninit(found)};
    *reinterpret_cast<ValT*>(jl_data_ptr(val)) =
        ValT(std::forward<ArgTs>(args_)...);
    return val;
}

inline value eval(generic_string src_str_)
{
    jl_value_t* res{jl_eval_string(src_str_)};
    impl::check_err();
    return res;
}

inline value exec_from_file(generic_string file_name_)
{
    std::ifstream file{file_name_};
    if (!file.is_open())
        throw load_error{std::string{"Could not load script "}
                         + static_cast<const char*>(file_name_)};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return eval(buffer.str().c_str());
}

template<typename... ArgTs>
value call(function fn_, ArgTs&&... args_)
{
    constexpr std::size_t num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);
    impl::make_arg_vec<jl_value_t*, ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* res{jl_call(fn_, boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
    return res;
}

template<typename... ArgTs>
value call(generic_string fn_name_, ArgTs&&... args_)
{
    return call(function{fn_name_}, std::forward<ArgTs>(args_)...);
}

template<typename... ArgTs>
value function::operator()(ArgTs&&... args_)
{
    return call(*this, std::forward<ArgTs>(args_)...);
}

inline void init()
{
    jl_init();
}

inline void quit(int code_ = 0)
{
    jl_atexit_hook(code_);
    delete[] impl::synced_cpp_types;
    delete[] impl::synced_jl_types;
}

inline void raise_error(generic_string content_)
{
    jl_error(content_);
}

template<typename... ArgTs>
void raise_error(generic_string content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

// inline void use(generic_string module_)
//{
//    jl_eval_string((std::string{"using "} + module_).c_str());
//}

} // namespace jl
