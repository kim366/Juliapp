#pragma once

#include "Assert.hpp"
#include "GenericValue.hpp"
#include "Symbol.hpp"

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

#define JLPP_SYNC(cpp_type, julia_type)                                        \
    namespace jl::impl                                                         \
    {                                                                          \
    template<>                                                                 \
    struct types_match_impl<::cpp_type>                                        \
    {                                                                          \
        static auto type()                                                     \
        { /*TODO: Assert sizeof */                                             \
            static auto type = julia_type.value().c_val();                     \
            return type;                                                       \
        }                                                                      \
                                                                               \
        constexpr static bool synced = true;                                   \
    };                                                                         \
    }                                                                          \
    struct jlpp_dummy

template<typename...>
struct sync_force_resolve_impl;

template<typename T, typename... Ts>
struct sync_force_resolve_impl<T, Ts...>
{
    static void resolve()
    {
        (void)types_match_impl<T>::type();
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
