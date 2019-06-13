cmake_minimum_required (VERSION 3.8)
project(julia-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-Wall -Wextra -Wpedantic -Werror -Wfatal-errors)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}")

find_package(Julia)

add_definitions(${Julia_DEFINITIONS})
include_directories(${Julia_INCLUDE_DIRS})
link_directories(${Julia_LIBRARY_DIRS})

add_definitions(-DJULIA_ENABLE_THREADING=1)


add_executable(demo demo/demo.cpp)

add_executable(tests tests/Tests.cpp tests/ValueTests.cpp tests/FreeFunctionTests.cpp tests/FunctionTests.cpp tests/ModuleTests.cpp tests/GlobalTests.cpp)
target_compile_options(tests PUBLIC -fprofile-arcs -ftest-coverage)

include_directories(src julia/include/julia)

target_link_libraries(demo ${Julia_LIBRARIES})
target_link_libraries(tests ${Julia_LIBRARIES} gcov --coverage)

