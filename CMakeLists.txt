cmake_minimum_required (VERSION 3.8)
project(julia-cpp LANGUAGES CXX)

find_library(
        JULIA_LIB julia
        HINTS "julia/lib"
)

set(CMAKE_CXX_STANDARD 17)

add_executable(demo demo/demo.cpp)
add_executable(tests tests/Tests.cpp tests/ValueTests.cpp tests/FreeFunctionTests.cpp tests/FunctionTests.cpp tests/ModuleTests.cpp)
target_compile_options(tests PUBLIC -DJULIA_ENABLE_THREADING=1 -fPIC -fprofile-arcs -ftest-coverage)
target_compile_options(demo PUBLIC -DJULIA_ENABLE_THREADING=1 -fPIC)
include_directories(src julia/include/julia)

target_link_libraries(demo ${JULIA_LIB})
target_link_libraries(tests ${JULIA_LIB} gcov --coverage)

add_definitions(-Wall -Wextra -Wconversion -pedantic)
