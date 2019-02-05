cmake_minimum_required (VERSION 3.8)
project(julia-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(demo demo/demo.cpp)
add_executable(tests tests/Tests.cpp tests/ValueTests.cpp tests/FreeFunctionTests.cpp)
target_compile_options(tests PUBLIC -DJULIA_ENABLE_THREADING=1 -fPIC)
target_compile_options(demo PUBLIC -DJULIA_ENABLE_THREADING=1 -fPIC)
include_directories(src julia/include/julia)

target_link_libraries(tests "-L \"julia/lib\"" julia)
target_link_libraries(demo  "-L \"julia/lib\"" julia)

add_definitions(-Wall -Wextra -Wconversion -pedantic)
