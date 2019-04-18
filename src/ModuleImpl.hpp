#include <Global.hpp>
#include <Module.hpp>

namespace jl
{

inline global module::operator[](symbol name_)
{
    jl_binding_t* binding = nullptr;

    JL_TRY { binding = jl_get_binding_wr(c_mod(), name_.c_sym(), true); }
    JL_CATCH { throw language_error{"Failed to get binding to symbol"}; }

    return binding;
}

} // namespace jl