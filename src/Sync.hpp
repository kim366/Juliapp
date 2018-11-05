#pragma once

#include "Assert.hpp"
#include "GenericString.hpp"

#include <julia.h>
#include <map>
#include <string>
#include <typeindex>

namespace jl
{

namespace impl
{

inline std::type_index* synced_cpp_types;
inline jl_datatype_t** synced_jl_types;
inline std::size_t num_synced_types;

template<typename CppT>
jl_datatype_t* find_synced_jl_type()
{
    std::type_index cpp_type{typeid(CppT)};
    for (std::size_t i = 0; i < num_synced_types; ++i)
    {
        if (synced_cpp_types[i] == cpp_type)
            return synced_jl_types[i];
    }

    return nullptr;
}

template<typename CppT>
bool verify_synced_cpp_type(jl_datatype_t* julia_type_)
{
    for (std::size_t i = 0; i < num_synced_types; ++i)
    {
        if (synced_jl_types[i] == julia_type_)
            return synced_cpp_types[i] == typeid(CppT);
    }

    return false;
}

} // namespace impl

template<typename CppT>
struct type
{
    using cpp_type = CppT;
    type(generic_string type_name_) : julia_type{type_name_} {}

    const char* julia_type;
};

template<typename... TypeTs>
void sync(TypeTs&&... ts_)
{
    if (impl::num_synced_types != 0)
    {
        delete[] impl::synced_cpp_types;
        delete[] impl::synced_jl_types;
    }
    impl::num_synced_types = sizeof...(ts_);
    impl::synced_cpp_types = new std::type_index[sizeof...(ts_)]{
        typeid(typename TypeTs::cpp_type)...};
    // TODO: improve performance by exchanging `jl_eval_string`
    impl::synced_jl_types = new jl_datatype_t* [sizeof...(ts_)] {
        reinterpret_cast<jl_datatype_t*>(jl_eval_string(ts_.julia_type))...
    };

#ifndef NDEBUG
    jl_function_t* sizeof_func{
        jl_get_global(jl_core_module, jl_symbol("sizeof"))};
    jl_function_t* isbits_check_func{nullptr};
    JL_GC_PUSH2(sizeof_func, isbits_check_func);
    isbits_check_func = jl_eval_string(R"(
      typ::DataType -> begin
        isbitstype(typ) && return true
        for n in fieldnames(typ)
          isbitstype(fieldtype(typ, n)) || return false;
        end
        true
      end
    )");

    jlpp_assert("Synced datatype sizes do not match"
                && ((sizeof(typename TypeTs::cpp_type)
                     == static_cast<std::size_t>(jl_unbox_int64(jl_call1(
                            sizeof_func, jl_eval_string(ts_.julia_type)))))
                    && ...));

    jlpp_assert((("Contents of synced types are not isbits"
                  && jl_unbox_bool(jl_call1(isbits_check_func,
                                            jl_eval_string(ts_.julia_type))))
                 && ...));

    JL_GC_POP();

#endif
}

} // namespace jl
