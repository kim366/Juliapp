#pragma once

#include <julia.h>
#include <map>
#include <string>
#include <typeindex>

namespace jl
{

namespace impl
{
std::map<std::type_index, jl_datatype_t*> type_map;
}

template<typename T>
void sync(const std::string& data_type_)
{
    // TODO: improve performance by exchanging `jl_eval_string`
    impl::type_map[typeid(T)] =
        reinterpret_cast<jl_datatype_t*>(jl_eval_string(data_type_.c_str()));
}

} // namespace jl
