#pragma once

#include "value.hpp"
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