/*

Juliapp
=======
Seamlessly embed Julia code in C++ with an API using modern language features.
https://github.com/kim366/Juliapp

=======

BSD 2-Clause License

Copyright (c) 2018, Kim Schmider
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <stdexcept>

namespace jl
{

struct error : std::runtime_error
{
protected:
    using std::runtime_error::runtime_error;
    virtual ~error(){};
};

struct language_error : error
{
    using error::error;
    virtual ~language_error() {}
};

struct result_type_error : error
{
    using error::error;
    virtual ~result_type_error() {}
};

struct load_error : error
{
    using error::error;
    virtual ~load_error() {}
};

} // namespace jl


#include <julia.h>

namespace jl
{

template<typename ElemT>
class array;

namespace impl
{

inline void check_err()
{
    jl_value_t* error{jl_exception_occurred()};
    if (error)
    {
#ifdef JULIAPP_DEBUG
        jl_value_t* show_error = jl_get_function(jl_base_module, "showerror");
        jl_call2(show_error, jl_stderr_obj(), error);
        jl_printf(JL_STDERR, "\n");
#endif
        throw language_error{jl_typeof_str(error)};
    }
}

template<typename ElemT>
jl_datatype_t* get_type()
{
    if constexpr (std::is_same_v<ElemT, std::int8_t>)
        return jl_int8_type;
    else if constexpr (std::is_same_v<ElemT, std::uint8_t>)
        return jl_uint8_type;
    else if constexpr (std::is_same_v<ElemT, std::int16_t>)
        return jl_int16_type;
    else if constexpr (std::is_same_v<ElemT, std::uint16_t>)
        return jl_uint16_type;
    else if constexpr (std::is_same_v<ElemT, std::int32_t>)
        return jl_int32_type;
    else if constexpr (std::is_same_v<ElemT, std::uint32_t>)
        return jl_uint32_type;
    else if constexpr (std::is_same_v<ElemT, std::int64_t>)
        return jl_int64_type;
    else if constexpr (std::is_same_v<ElemT, std::uint64_t>)
        return jl_uint64_type;
    else if constexpr (std::is_same_v<ElemT, float>)
        return jl_float32_type;
    else if constexpr (std::is_same_v<ElemT, double>)
        return jl_float64_type;
    else if constexpr (std::is_same_v<ElemT, bool>)
        return jl_bool_type;
    else
    {
        assert(false &&
               "jl - unsupported array type. "
               "Use boolean, floating point or integral types.");
    }
}

template<typename>
struct is_array : std::false_type
{
};

template<typename ElemT>
struct is_array<array<ElemT>> : std::true_type
{
};

template<typename T>
class is_convertible
{
    template<typename C, typename = decltype(convert(std::declval<C>()))>
    static std::true_type test(int);

    template<typename>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

} // namespace impl

} // namespace jl


#include <julia.h>

namespace jl
{

template<typename ElemT>
class array;

namespace impl
{

template<typename RetT>
RetT unbox(jl_value_t* arg_)
{
    if constexpr (std::is_floating_point_v<RetT>)
    {
        if (jl_typeis(arg_, jl_float32_type))
            return jl_unbox_float32(arg_);
        else if (jl_typeis(arg_, jl_float64_type))
            return jl_unbox_float64(arg_);

        throw result_type_error{
            "Incorrect result type. Floating-point type requested"};
    }
    else if constexpr (std::is_integral_v<RetT>)
    {
        if constexpr (std::is_signed_v<RetT>)
        {
            if (jl_typeis(arg_, jl_int8_type))
                return jl_unbox_int8(arg_);
            else if (jl_typeis(arg_, jl_int16_type))
                return jl_unbox_int16(arg_);
            else if (jl_typeis(arg_, jl_int32_type))
                return jl_unbox_int32(arg_);
            else if (jl_typeis(arg_, jl_int64_type))
                return jl_unbox_int64(arg_);
        }
        else
        {
            if (jl_typeis(arg_, jl_uint8_type))
                return jl_unbox_uint8(arg_);
            else if (jl_typeis(arg_, jl_uint16_type))
                return jl_unbox_uint16(arg_);
            else if (jl_typeis(arg_, jl_uint32_type))
                return jl_unbox_uint32(arg_);
            else if (jl_typeis(arg_, jl_uint64_type))
                return jl_unbox_uint64(arg_);
        }

        throw result_type_error{
            "Incorrect result type. Integral type requested"};
    }
    else if constexpr (std::is_same_v<RetT, bool>)
    {
        if (jl_typeis(arg_, jl_bool_type))
            return jl_unbox_bool(arg_);

        throw result_type_error{
            "Incorrect result type. Boolean type requested"};
    }
    else
    {
        assert(false &&
               "jl - unsupported result type. "
               "Use boolean, floating point or integral types.");
    }
}

template<typename ArgT>
jl_value_t* box(ArgT& arg_)
{
    if constexpr (std::is_convertible_v<ArgT, jl_value_t*>)
        return arg_;
    else if constexpr (impl::is_convertible<ArgT>::value)
        return convert(arg_);
    else if constexpr (std::is_same<ArgT, bool>())
        return jl_box_bool(arg_);
    else if constexpr (std::is_same<ArgT, std::int8_t>())
        return jl_box_int8(arg_);
    else if constexpr (std::is_same<ArgT, std::uint8_t>())
        return jl_box_uint8(arg_);
    else if constexpr (std::is_same<ArgT, std::int16_t>())
        return jl_box_int16(arg_);
    else if constexpr (std::is_same<ArgT, std::uint16_t>())
        return jl_box_uint16(arg_);
    else if constexpr (std::is_same<ArgT, std::int32_t>())
        return jl_box_int32(arg_);
    else if constexpr (std::is_same<ArgT, std::uint32_t>())
        return jl_box_uint32(arg_);
    else if constexpr (std::is_same<ArgT, std::int64_t>())
        return jl_box_int64(arg_);
    else if constexpr (std::is_same<ArgT, std::uint64_t>())
        return jl_box_uint64(arg_);
    else if constexpr (std::is_same<ArgT, float>())
        return jl_box_float32(arg_);
    else if constexpr (std::is_same<ArgT, double>())
        return jl_box_float64(arg_);
    else if constexpr (std::is_same<ArgT, void*>())
        return jl_box_voidpointer(arg_);
    else if constexpr (is_array<ArgT>{})
        return reinterpret_cast<jl_value_t*>(arg_.get_boxed_data());
    else if constexpr (!std::is_fundamental_v<std::decay_t<ArgT>>)
    {
        if constexpr (std::is_pointer_v<ArgT>)
            return reinterpret_cast<jl_value_t*>(arg_);
        else
            return reinterpret_cast<jl_value_t*>(&arg_);
    }
    else
    {
        assert(false &&
               "jl - unsupported argument type. "
               "Use array, boolean, floating point or integral types.");
    }
}

template<typename...>
struct make_arg_vec;

template<typename FirstArgT, typename... RestArgTs>
struct make_arg_vec<FirstArgT, RestArgTs...>
{
    static void make(jl_value_t** vector_,
                     std::size_t index_,
                     FirstArgT first_,
                     RestArgTs... rest_)
    {
        vector_[index_] = box(first_);
        make_arg_vec<RestArgTs...>::make(vector_, index_ + 1, rest_...);
    }
};

template<>
struct make_arg_vec<>
{
    static void make(jl_value_t**, std::size_t) {}
};

} // namespace impl

} // namespace jl


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

    operator jl_value_t*() { return _boxed_value; }

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
value call(const char* fn_name_, ArgTs&&... args_)
{
    constexpr std::size_t num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);

    impl::make_arg_vec<ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* func{jl_eval_string(fn_name_)};
    impl::check_err();
    jl_value_t* res{jl_call(func, boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
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

inline void use(const std::string& module_)
{
    jl_eval_string((std::string{"using "} + module_).c_str());
}

inline void use(const char* module_)
{
    use(std::string{module_});
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
