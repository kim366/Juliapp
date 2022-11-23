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
    value operator*() const;
    global& operator=(const value& val);
};

global global::from_raw(jl_binding_t* val)
{
    auto result = global{};
    result.raw_ = val;
    return result;
}

value global::operator*() const
{
    return value::from_raw(raw_->value);
}

//global& global::operator=(const value& val)
//{
//    // TODO: Base.cast
//    jl_checked_assignment(raw_, val.raw());
//    return *this;
//}

} // namespace jl