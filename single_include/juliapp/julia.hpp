/*

Juliapp
=======
Seamlessly embed Julia code in C++ with an API using modern language features.
https://github.com/kim366/Juliapp

=======

BSD 2-Clause License

Copyright (c) 2018-2019, Kim Schmider
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

#include <cassert>
#include <stdexcept>

#ifdef JLPP_IMPL_UNIT_TESTING

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
#define jlpp_assert(expr)                                                      \
    jl::impl::test::assertion_instance{expr, #expr}
#define jlpp_static_assert(expr, ...)                                          \
    jl::impl::test::static_assertion_instance<expr>{#__VA_ARGS__}
// clang-format on

#else
#define jlpp_assert(expr) assert(expr)
#define jlpp_static_assert(expr, ...) static_assert(expr, __VA_ARGS__)
#endif


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


#include <julia.h>

namespace jl
{

template<typename ElemT>
class array;

template<typename ValT>
class value;

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

template<typename>
struct is_array : std::false_type
{
};

template<typename ElemT>
struct is_array<array<ElemT>> : std::true_type
{
};

template<typename>
struct is_value : std::false_type
{
};

template<typename ValT>
struct is_value<value<ValT>> : std::true_type
{
};

} // namespace impl

} // namespace jl

#include <julia.h>
#include <utility>

namespace jl
{

class symbol;
class generic_value;
struct function;
struct module;

class global
{
public:
    global(jl_binding_t* binding_);
    global(symbol symbol_);
    global(const global&) = default;

    jl_binding_t* c_binding();
    generic_value value();
    function as_function();
    module as_module();

    template<
        typename T,
        typename NoRefT = std::remove_reference_t<T>,
        std::enable_if_t<!impl::is_value<NoRefT>{}
                         && !std::is_same_v<NoRefT, generic_value>>* = nullptr>
    global& operator=(T&& data_);

    global& operator=(const generic_value& value_);

    template<typename TargT>
    operator TargT();

private:
    jl_binding_t* _binding;
};

} // namespace jl


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

    generic_value generic() const&;
    generic_value generic() &&;

    function as_function();
    module as_module();

protected:
    jl_value_t* _boxed_value;
};

} // namespace jl


#include <julia.h>
#include <string>

namespace jl
{

class symbol
{
public:
    symbol(jl_sym_t* symbol_) : _symbol{symbol_} {}
    symbol(const std::string& name_) : symbol{name_.c_str()} {}
    symbol(const char* name_) : symbol{jl_symbol(name_)} {}

    jl_sym_t* c_sym() { return _symbol; }

private:
    jl_sym_t* _symbol;
};

} // namespace jl

#include <cassert>
#include <julia.h>

namespace jl
{

namespace impl
{

template<typename SyncedT>
struct types_match_impl
{
    constexpr static bool synced = false;
    static jl_value_t* type() { return nullptr; }
};

#define IMPL_JLPP_ASSERT_SYNC                                                  \
    jlpp_static_assert(::jl::impl::types_match_impl<SyncedT>::synced,          \
                       "Type not synced (type labelled as SyncedT)!")

#define JLPP_SYNC(cpp_type, julia_type)                                                            \
    namespace jl::impl                                                                             \
    {                                                                                              \
    template<>                                                                                     \
    struct types_match_impl<::cpp_type>                                                            \
    {                                                                                              \
        static auto type()                                                                         \
        {                                                                                          \
            static auto type = julia_type.value().c_val();                                         \
            jlpp_assert(                                                                           \
                jl_datatype_size(type) == sizeof(cpp_type)                                         \
                && "Synced type sizes do not match. Are you sure the definitions are identical?"); \
            return type;                                                                           \
        }                                                                                          \
                                                                                                   \
        constexpr static bool synced = true;                                                       \
    };                                                                                             \
    }                                                                                              \
    static_assert(std::is_standard_layout_v<::cpp_type>,                                           \
                  "The synced type " #cpp_type                                                     \
                  " needs to have standard layout.")

template<typename...>
struct sync_force_resolve_impl;

template<typename T, typename... Ts>
struct sync_force_resolve_impl<T, Ts...>
{
    static void resolve()
    {
        static_cast<void>(types_match_impl<T>::type());
        sync_force_resolve_impl<Ts...>::resolve();
    }
};

template<>
struct sync_force_resolve_impl<>
{
    static void resolve() {}
};

template<typename SyncedT>
jl_datatype_t* find_synced_jl_type()
{
    IMPL_JLPP_ASSERT_SYNC;
    return reinterpret_cast<jl_datatype_t*>(
        impl::types_match_impl<SyncedT>::type());
}

template<typename SyncedT>
bool types_match(generic_value name)
{
    IMPL_JLPP_ASSERT_SYNC;
    return impl::types_match_impl<SyncedT>::type() == name.c_val();
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
        return find_synced_jl_type<ElemT>();
}

} // namespace impl

template<typename... Ts>
void sync_force_resolve()
{
    impl::sync_force_resolve_impl<Ts...>::resolve();
}

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
    else if constexpr (std::is_same_v<RetT, bool>)
    {
        if (jl_typeis(arg_, jl_bool_type))
            return jl_unbox_bool(arg_);

        throw result_type_error{
            "Incorrect result type. Boolean type requested"};
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
    else
    {
        if (!impl::types_match<std::decay_t<RetT>>(jl_typeof(arg_)))
            throw result_type_error{
                std::string{"Incorrect type requested. "}
                + jl_typeof_str(reinterpret_cast<jl_value_t*>(arg_))
                + " given, but synced with "
                + jl_typename_str(reinterpret_cast<jl_value_t*>(
                      find_synced_jl_type<std::decay_t<RetT>>()))
                + '.'};
        return *reinterpret_cast<std::decay_t<RetT>*>(jl_data_ptr(arg_));
    }
}

template<typename ArgT>
jl_value_t* box(ArgT&& arg_)
{
    using DecayedArgT = std::decay_t<ArgT>;
    using ArgDeclT = decltype(arg_);

    if constexpr (std::is_convertible_v<DecayedArgT, jl_value_t*>)
        return arg_;
    else if constexpr (std::is_base_of_v<generic_value, DecayedArgT>)
        return arg_.c_val();
    else if constexpr (std::is_same_v<global, DecayedArgT>)
        return arg_.value().c_val();
    else if constexpr (std::is_same<DecayedArgT, bool>())
        return jl_box_bool(arg_);
    else if constexpr (std::is_integral_v<DecayedArgT>)
    {
        if constexpr (std::is_signed_v<DecayedArgT>)
        {
            if constexpr (sizeof(ArgDeclT) == 1)
                return jl_box_int8(arg_);
            else if constexpr (sizeof(ArgDeclT) == 2)
                return jl_box_int16(arg_);
            else if constexpr (sizeof(ArgDeclT) == 4)
                return jl_box_int32(arg_);
            else if constexpr (sizeof(ArgDeclT) == 8)
                return jl_box_int64(arg_);
        }
        else
        {
            if constexpr (sizeof(ArgDeclT) == 1)
                return jl_box_uint8(arg_);
            else if constexpr (sizeof(ArgDeclT) == 2)
                return jl_box_uint16(arg_);
            else if constexpr (sizeof(ArgDeclT) == 4)
                return jl_box_uint32(arg_);
            else if constexpr (sizeof(ArgDeclT) == 8)
                return jl_box_uint64(arg_);
        }
    }
    else if constexpr (std::is_same<DecayedArgT, float>())
        return jl_box_float32(arg_);
    else if constexpr (std::is_same<DecayedArgT, double>())
        return jl_box_float64(arg_);
    else if constexpr (std::is_same<DecayedArgT, void*>())
        return jl_box_voidpointer(arg_);
    else if constexpr (is_array<DecayedArgT>{})
        return reinterpret_cast<jl_value_t*>(arg_.get_boxed_data());
    else
    {
        jl_datatype_t* found{impl::find_synced_jl_type<DecayedArgT>()};
        jlpp_assert(found && "Requested type not synced");
        jl_value_t* val{jl_new_struct_uninit(found)};
        new (jl_data_ptr(val)) ArgT{std::forward<ArgT>(arg_)};
        return val;
    }
}

template<typename...>
struct make_arg_vec;

template<typename DatT, typename FirstArgT, typename... RestArgTs>
struct make_arg_vec<DatT, FirstArgT, RestArgTs...>
{
    static void make(DatT* vector_,
                     std::size_t index_,
                     FirstArgT first_,
                     RestArgTs... rest_)
    {
        if constexpr (std::is_same_v<DatT, jl_value_t*>)
        {
            jl_value_t* boxed_val{box(first_)};
            vector_[index_] = boxed_val;
        }
        else
            vector_[index_] = first_;

        make_arg_vec<DatT, RestArgTs...>::make(vector_, index_ + 1, rest_...);
    }
};

template<typename DatT>
struct make_arg_vec<DatT>
{

    static void make(DatT*, std::size_t) {}
};

} // namespace impl

} // namespace jl


#include <utility>


#include <algorithm>
#include <julia.h>
#include <julia_gcext.h>
#include <vector>

namespace jl::impl
{

inline std::vector<jl_value_t*> rooted_values;

void root_scanner_cb(int);

inline void init()
{
    jl_init();
    jl_gc_set_cb_root_scanner(root_scanner_cb, true);
}

inline void quit(int status_ = 0)
{
    rooted_values.clear();
    rooted_values.shrink_to_fit();
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
        jlpp_assert(found_val != rooted_values.rend()
                    && "Releasing unrooted value");
        rooted_values.erase(std::next(found_val).base());
    }
}

void ensure_init();

class init_guard
{
    init_guard() noexcept { init(); }
    ~init_guard() { quit(); }

    friend void impl::ensure_init();
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
        jl_gc_mark_queue_obj(jl_get_ptls_states(), val);
}

} // namespace jl::impl

#include <julia.h>
#include <utility>

namespace jl
{

class symbol;
class global;

struct module : generic_value
{
    module(generic_value val_);
    module(symbol name_);

    global operator[](symbol name_) const;

    jl_module_t* c_mod() const;

    bool operator==(const module& rhs) const;
    bool operator!=(const module& rhs) const;
};

namespace impl
{

global get_binding(jl_module_t* mod, symbol name_);

} // namespace impl

#ifndef JLPP_MANUAL_INIT
const module main = generic_value{
    (impl::ensure_init(), reinterpret_cast<jl_value_t*>(jl_main_module))};
#endif

} // namespace jl


#include <julia.h>
#include <string>

namespace jl
{

struct function : public generic_value
{
    function(generic_value val_) : generic_value{std::move(val_)} {}

    function(symbol symbol_)
        : generic_value{jl_get_global(jl_main_module, symbol_.c_sym())}
    {
    }

    jl_function_t* c_fn() const { return c_val(); }

    template<typename... ArgTs>
    generic_value operator()(ArgTs&&... args_);

    bool operator==(const function& rhs) const { return c_fn() == rhs.c_fn(); }

    bool operator!=(const function& rhs) const { return !(rhs == *this); }
};

namespace impl
{

template<typename... ArgTs>
generic_value call(function fn_, ArgTs&&... args_)
{
    impl::ensure_init();

    if (fn_.c_fn() == nullptr)
        throw language_error{"MethodError"};
    constexpr int num_args{sizeof...(args_)};
    jl_value_t** boxed_args;
    JL_GC_PUSHARGS(boxed_args, num_args);
    impl::make_arg_vec<jl_value_t*, ArgTs...>::make(boxed_args, 0, args_...);

    jl_value_t* res{jl_call(fn_.c_fn(), boxed_args, num_args)};
    impl::check_err();
    JL_GC_POP();
    return res;
}

} // namespace impl

template<typename... ArgTs>
generic_value function::operator()(ArgTs&&... args_)
{
    return impl::call(*this, std::forward<ArgTs>(args_)...);
}

} // namespace jl


namespace jl
{

struct function;
struct module;

namespace impl
{

} // namespace impl

template<typename ValT>
struct value : public generic_value
{
    value(ValT&& obj_) : generic_value{impl::box(std::forward<ValT>(obj_))} {}
    value(const ValT& obj_) : generic_value{impl::box(obj_)} {}

    template<typename std::enable_if_t<std::is_fundamental_v<ValT>>* = nullptr>
    ValT operator*();

    template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>* = nullptr>
    ValT& operator*();

    template<typename std::enable_if_t<!std::is_fundamental_v<ValT>>* = nullptr>
    const ValT& operator*() const;

    ValT* operator->();
};

} // namespace jl

namespace jl
{

inline function generic_value::as_function()
{
    return function(*this);
}

inline module generic_value::as_module()
{
    return module{*this};
}

inline generic_value global::value()
{
    assert(c_binding()->value != nullptr);
    return c_binding()->value;
}

inline function global::as_function()
{
    return value().as_function();
}

inline module global::as_module()
{
    return value().as_module();
}

template<typename TargT>
global::operator TargT()
{
    return value().get<TargT>();
}

} // namespace jl


namespace jl
{

inline generic_value::generic_value(jl_value_t* boxed_value_) noexcept
    : _boxed_value{boxed_value_}
{
    impl::root_value(_boxed_value);
}

inline generic_value::generic_value() noexcept : generic_value{nullptr} {}
inline generic_value::generic_value(const generic_value& other)
    : generic_value{other.c_val()}
{
}

inline generic_value::generic_value(generic_value&& other)
    : _boxed_value{other.c_val()}
{
    other._boxed_value = nullptr;
}

inline generic_value::~generic_value()
{
    impl::release_value(_boxed_value);
}

inline jl_value_t* generic_value::c_val() const
{
    return _boxed_value;
}

inline bool generic_value::operator==(const generic_value& rhs) const
{
    return static_cast<bool>(jl_egal(_boxed_value, rhs._boxed_value));
}

inline bool generic_value::operator!=(const generic_value& rhs) const
{
    return !(rhs == *this);
}

template<
    typename TargT,
    std::enable_if_t<std::is_fundamental<TargT>{} || impl::is_array<TargT>{}>*>
TargT generic_value::get()
{
    if constexpr (std::is_integral_v<TargT>)
        return static_cast<TargT>(impl::unbox<long>(_boxed_value));
    else if constexpr (std::is_floating_point_v<TargT>)
        return static_cast<TargT>(impl::unbox<double>(_boxed_value));
    else
        return *this;
}

template<typename TargT, std::enable_if_t<!std::is_fundamental<TargT>{}>*>
TargT generic_value::get() noexcept
{
    if constexpr (std::is_pointer_v<TargT>)
        return reinterpret_cast<TargT>(_boxed_value);
    else
        return *reinterpret_cast<std::decay_t<TargT>*>(_boxed_value);
}

template<typename TargT, typename>
generic_value::operator TargT()
{
    return impl::unbox<TargT>(_boxed_value);
}

template<typename TargT,
         std::enable_if_t<std::is_class_v<std::decay_t<TargT>>>*>
generic_value::operator TargT()
{
    return get<TargT>();
}

template<typename ElemT>
inline generic_value::operator array<ElemT>() noexcept
{
    return reinterpret_cast<jl_array_t*>(_boxed_value);
}

inline generic_value generic_value::generic() const&
{
    return generic_value{*this};
}

inline generic_value generic_value::generic() &&
{
    return generic_value{std::move(*this)};
}

} // namespace jl


#include <julia.h>

namespace jl
{

inline global::global(jl_binding_t* binding_) : _binding{binding_} {}
inline global::global(symbol symbol_)
    : global{(impl::ensure_init(), impl::get_binding(jl_main_module, symbol_))}
{
}

inline jl_binding_t* global::c_binding()
{
    return _binding;
}

template<typename T,
         typename NoRefT,
         std::enable_if_t<!impl::is_value<NoRefT>{}
                          && !std::is_same_v<NoRefT, generic_value>>*>
global& global::operator=(T&& data_)
{
    return *this = ::jl::value<T>{std::forward<T>(data_)};
}

inline global& global::operator=(const generic_value& value_)
{
    jl_checked_assignment(_binding, value_.c_val());
    return *this;
}

} // namespace jl


namespace jl::literals
{
inline function operator""_jlf(const char* name_, std::size_t)
{
    return function{name_};
}

inline module operator""_jlm(const char* name_, std::size_t)
{
    return module{name_};
}

inline global operator""_jlg(const char* name_, std::size_t)
{
    return global{name_};
}

} // namespace jl::literals


namespace jl
{

inline module::module(generic_value val_) : generic_value{std::move(val_)} {}

inline module::module(symbol name_)
    : generic_value{jl_get_global(jl_main_module, name_.c_sym())}
{
}

inline global module::operator[](symbol name_) const
{
    return impl::get_binding(c_mod(), name_);
}

inline jl_module_t* module::c_mod() const
{
    return reinterpret_cast<jl_module_t*>(c_val());
}

inline bool module::operator==(const module& rhs) const
{
    return c_val() == rhs.c_val();
}

inline bool module::operator!=(const module& rhs) const
{
    return !(rhs == *this);
}

namespace impl
{

inline global get_binding(jl_module_t* mod, symbol name_)
{
    jl_binding_t* binding = nullptr;

    JL_TRY { binding = jl_get_binding_wr(mod, name_.c_sym(), true); }
    JL_CATCH { throw language_error{"Failed to get binding to symbol"}; }

    return binding;
}

} // namespace impl

} // namespace jl


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

#include <array>
#include <fstream>
#include <iostream>
#include <julia.h>
#include <sstream>
#include <string>

namespace jl
{

template<typename ValT, typename... ArgTs>
value<ValT> make_value(ArgTs&&... args_)
{
    jl_datatype_t* found{impl::find_synced_jl_type<ValT>()};
    jlpp_assert(found && "Requested type not synced");
    jl_value_t* val{jl_new_struct_uninit(found)};
    *reinterpret_cast<ValT*>(jl_data_ptr(val)) =
        ValT(std::forward<ArgTs>(args_)...);
    return val;
}

inline generic_value eval(std::string_view src_str_)
{
    impl::ensure_init();
    jl_value_t* res{jl_eval_string(src_str_.data())};
    impl::check_err();
    return res;
}

inline generic_value exec_from_file(std::string_view file_name_)
{
    std::ifstream file{file_name_.data()};
    if (!file.is_open())
        throw load_error{std::string{"Could not load script "}
                         + file_name_.data()};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return eval(buffer.str().c_str());
}

inline void raise_error(std::string_view content_)
{
    jl_error(content_.data());
}

template<typename... ArgTs>
void raise_error(symbol content_, ArgTs... args_)
{
    jl_errorf(content_, args_...);
}

inline void init()
{
#ifndef JLPP_MANUAL_INIT
    jlpp_assert(false && "Define JLPP_MANUAL_INIT before using init/quit");
#endif
    impl::init();
}

inline void quit(int status_ = 0)
{
#ifndef JLPP_MANUAL_INIT
    jlpp_assert(false && "Define JLPP_MANUAL_INIT before using init/quit");
#endif
    impl::quit(status_);
}

// inline void use(symbol module_)
//{
//    jl_eval_string((std::string{"using "} + module_).c_str());
//}

using int_t =
    std::conditional_t<sizeof(void*) == 8, std::int64_t, std::int32_t>;
using uint_t =
    std::conditional_t<sizeof(void*) == 8, std::uint64_t, std::uint32_t>;

} // namespace jl

#endif // JULIAPP_SINGLE_INCLUDE_HPP
