cmake_minimum_required (VERSION 3.12)
project(juliapp LANGUAGES CXX)

aux_source_directory(src sources)
add_executable(demo ${sources})
target_compile_options(demo PUBLIC -I/usr/include/julia -DJULIA_ENABLE_THREADING=1 -fPIC)
target_include_directories(demo PUBLIC src)

set_target_properties(demo PROPERTIES CXX_STANDARD 20)
target_link_libraries(demo -L'/usr/lib' -Wl,--export-dynamic -Wl,-rpath,'/usr/lib' -Wl,-rpath,'/usr/lib/julia' julia)

add_definitions(-Wall -Wextra -Wconversion -pedantic -fconcepts)
