cmake_minimum_required (VERSION 3.8)
project(julia-cpp LANGUAGES CXX)

aux_source_directory(src sources)
add_executable(main ${sources})
target_compile_options(main PUBLIC -DJULIA_ENABLE_THREADING=1 -fPIC)
target_include_directories(main PUBLIC src)

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
set_target_properties(main PROPERTIES CXX_STANDARD 17)
target_link_libraries(main -L'/usr/lib' -Wl,--export-dynamic -Wl,-rpath,'/usr/lib' -Wl,-rpath,'/usr/lib/julia' julia)

add_definitions(-Wall -Wextra -Wconversion -pedantic)
