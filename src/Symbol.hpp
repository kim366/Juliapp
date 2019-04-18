#pragma once

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