/*

Juliapp
=======
Seamlessly embed Julia code in C++ with an API using modern language features.
https://github.com/kim366/Juliapp

=======

BSD 2-Clause License

Copyright (c) 2018-2022, Kim Schmider
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

#ifndef JULIAPP_SINGLE_INCLUDE_HPP
#define JULIAPP_SINGLE_INCLUDE_HPP

#include <stdexcept>

namespace jl
{

struct error : std::runtime_error
{
protected:
    using std::runtime_error::runtime_error;

public:
    ~error() override = default;
};

struct language_error : error
{
    using error::error;
    ~language_error() override = default;
};

struct result_type_error : error
{
    using error::error;
    ~result_type_error() override = default;
};

struct load_error : error
{
    using error::error;
    ~load_error() override = default;
};

} // namespace jl

#include <julia/julia.h>

namespace jl::impl
{

template<typename T>
T unbox(jl_value_t* val)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        if (jl_typeis(val, jl_float32_type))
            return jl_unbox_float32(val);
        else if (jl_typeis(val, jl_float64_type))
            return jl_unbox_float64(val);

        throw result_type_error{
            "Incorrect result type. Floating-point type requested"};
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        if (jl_typeis(val, jl_bool_type))
            return jl_unbox_bool(val);

        throw result_type_error{
            "Incorrect result type. Boolean type requested"};
    }
    else if constexpr (std::is_integral_v<T>)
    {
        if constexpr (std::is_signed_v<T>)
        {
            if (jl_typeis(val, jl_int8_type))
                return static_cast<T>(jl_unbox_int8(val));
            else if (jl_typeis(val, jl_int16_type))
                return static_cast<T>(jl_unbox_int16(val));
            else if (jl_typeis(val, jl_int32_type))
                return static_cast<T>(jl_unbox_int32(val));
            else if (jl_typeis(val, jl_int64_type))
                return static_cast<T>(jl_unbox_int64(val));
        }
        else
        {
            if (jl_typeis(val, jl_uint8_type))
                return static_cast<T>(jl_unbox_uint8(val));
            else if (jl_typeis(val, jl_uint16_type))
                return static_cast<T>(jl_unbox_uint16(val));
            else if (jl_typeis(val, jl_uint32_type))
                return static_cast<T>(jl_unbox_uint32(val));
            else if (jl_typeis(val, jl_uint64_type))
                return static_cast<T>(jl_unbox_uint64(val));
        }

        throw result_type_error{
            "Incorrect result type. Integral type requested"};
    }
    else
    {
        throw result_type_error{"unsupported type"};
    }
}

template<typename T>
jl_value_t* box(const T& val)
{
    if constexpr (std::is_same<T, bool>())
        return jl_box_bool(val);
    else if constexpr (std::is_integral_v<T>)
    {
        if constexpr (std::is_signed_v<T>)
        {
            if constexpr (sizeof(T) == 1)
                return jl_box_int8(val);
            else if constexpr (sizeof(T) == 2)
                return jl_box_int16(val);
            else if constexpr (sizeof(T) == 4)
                return jl_box_int32(val);
            else if constexpr (sizeof(T) == 8)
                return jl_box_int64(val);
        }
        else
        {
            if constexpr (sizeof(T) == 1)
                return jl_box_uint8(val);
            else if constexpr (sizeof(T) == 2)
                return jl_box_uint16(val);
            else if constexpr (sizeof(T) == 4)
                return jl_box_uint32(val);
            else if constexpr (sizeof(T) == 8)
                return jl_box_uint64(val);
        }
    }
    else if constexpr (std::is_same<T, float>())
        return jl_box_float32(val);
    else if constexpr (std::is_same<T, double>())
        return jl_box_float64(val);
    else if constexpr (std::is_same<T, void*>())
        return jl_box_voidpointer(val);
    else
    {
       throw result_type_error{"unsupported type"};
    }
}

} // namespace jl::impl


#include <cassert>
#include <stdexcept>

#ifdef IMPL_JLPP_UNIT_TESTING

namespace jl
{

namespace test
{

struct failed_assertion : std::logic_error
{
    using std::logic_error::logic_error;

    virtual ~failed_assertion() noexcept {}
};

struct failed_static_assertion : std::logic_error
{
    using std::logic_error::logic_error;

    virtual ~failed_static_assertion() noexcept {}
};

} // namespace test

namespace impl::test
{

struct assertion_instance
{
    assertion_instance(bool expr_, const char* msg_)
    {
        if (!expr_)
            throw jl::test::failed_assertion{std::string{"Assertion failed: ("}
                                             + msg_ + ")"};
    }
};

template<bool>
struct static_assertion_instance;

template<>
struct static_assertion_instance<true>
{
    static_assertion_instance(const char*) {}
};

template<>
struct static_assertion_instance<false>
{
    static_assertion_instance(const char* reason_)
    {
        throw jl::test::failed_static_assertion{reason_};
    }
};
} // namespace impl::test

} // namespace jl

// clang-format off
#define impl_jlpp_assert(expr)                                                      \
    jl::impl::test::assertion_instance{expr, #expr}
#define impl_jlpp_static_assert(expr, ...)                                          \
    jl::impl::test::static_assertion_instance<expr>{#__VA_ARGS__}
// clang-format on

#else
#define impl_jlpp_assert(expr) assert(expr)
#define impl_jlpp_static_assert(expr, ...) static_assert(expr, __VA_ARGS__)
#endif

#define IMPL_JLPP_IGNORE_ERROR(expr)                                           \
    do                                                                         \
    {                                                                          \
        try                                                                    \
        {                                                                      \
            expr;                                                              \
        }                                                                      \
        catch (const jl::error&)                                               \
        {                                                                      \
        }                                                                      \
    } while (false)


namespace jl
{

void eval(const char* source)
{
    jl_eval_string(source);
}

} // namespace jl


#include <julia/julia.h>

namespace jl
{

class global
{
private:
    jl_binding_t* raw_;

public:
    static global from_raw(jl_binding_t* val);
    value operator*();
//    global& operator=(const value& val);
};

global global::from_raw(jl_binding_t* val)
{
    auto result = global{};
    result.raw_ = val;
    return result;
}

value global::operator*()
{
    return value::from_raw(raw_->value);
}

} // namespace jl

#include <julia/julia.h>
#include <utility>

namespace jl
{

class module
{
private:
    jl_module_t* raw_;

public:
    explicit module(const value& val);

    // TODO: switch const char* to jl::symbol
    global operator[](const char* name) const;

    static module from_raw(jl_module_t* raw);

private:
    module(jl_module_t* raw);
};

module::module(jl_module_t* raw)
    : raw_{raw}
{
}

module::module(const value& val)
{
    if (!jl_is_module(val.raw()))
        jl_error("Value is not a module");

    raw_ = reinterpret_cast<jl_module_t*>(val.raw());
}

global module::operator[](const char* name) const
{
    return global::from_raw(jl_get_binding_wr(raw_, jl_symbol(name), 0));
}

module module::from_raw(jl_module_t* raw)
{
    return module{raw};
}

#ifndef JLPP_MANUAL_INIT
const auto main = module::from_raw(
    (impl::ensure_init(), jl_main_module));
#endif

} // namespace jl


namespace jl::literals
{

inline global operator""_jlg(const char* name_, std::size_t)
{
    return jl::main[name_];
}

inline value operator""_jlv(const char* name_, std::size_t)
{
    return *jl::main[name_];
}

} // namespace jl::literals


#include <algorithm>
#include <julia/julia.h>
#include <julia/julia_gcext.h>
#include <vector>

namespace jl::impl
{

inline std::vector<jl_value_t*> rooted_values;
inline jl_ptls_t ptls;

void root_scanner_cb(int);

inline void init()
{
    jl_init();
    ptls = static_cast<jl_ptls_t>(jl_get_ptls_states());
    jl_gc_set_cb_root_scanner(root_scanner_cb, true);
}

inline void quit(int status_ = 0)
{
    rooted_values = {};
    jl_atexit_hook(status_);
}

inline void root_value(jl_value_t* val)
{
    rooted_values.push_back(val);
}

inline void release_value(jl_value_t* val)
{
    if (val != nullptr)
    {
        const auto found_val =
            std::find(rooted_values.rbegin(), rooted_values.rend(), val);
        impl_jlpp_assert(found_val != rooted_values.rend()
                    && "Releasing unrooted value");
        rooted_values.erase(std::next(found_val).base());
    }
}

class init_guard
{
    init_guard() noexcept { init(); }
    ~init_guard() { quit(); }

    friend void ensure_init();
};

inline void ensure_init()
{
#ifndef JLPP_MANUAL_INIT
    static const auto guard = init_guard{};
#endif
}

inline void root_scanner_cb(int)
{
     for (jl_value_t* val : rooted_values)
         jl_gc_mark_queue_obj(ptls, val);
}

} // namespace jl::impl


#include <julia/julia.h>
#include <utility>

namespace jl
{

class value;

template<typename T>
T value_cast(value&& val);

template<typename T>
T value_cast(const value& val);

class value
{
private:
    jl_value_t* raw_;

public:
    value(const value& other);
    value& operator=(const value& other);

    value(value&& other);
    value& operator=(value&& other);

    ~value();

    static value from_raw(jl_value_t* val);
    jl_value_t *raw() const;

private:
    explicit value(jl_value_t* raw);
};

template<typename T>
T value_cast(value&& val)
{
    return value_cast<T>(val);
}

template<typename T>
T value_cast(const value& val)
{
    return jl::impl::unbox<T>(val.raw());
}

value::value(jl_value_t* raw)
    : raw_{raw}
{
    impl::root_value(raw_);
}

value::value(const value& other)
    : value{other.raw_}
{
}

value& value::operator=(const value& other)
{
    auto tmp = other;
    std::swap(tmp.raw_, raw_);
    return *this;
}

value::value(value&& other)
    : raw_{nullptr}
{
    std::swap(other.raw_, raw_);
}

value& value::operator=(value&& other)
{
    std::swap(other.raw_, raw_);
    return *this;
}

value::~value()
{
    if (raw_)
        impl::release_value(raw_);
}

value value::from_raw(jl_value_t* val)
{
    return value{val};
}

jl_value_t* value::raw() const
{
    return raw_;
}

} // namespace jl


#endif // JULIAPP_SINGLE_INCLUDE_HPP
