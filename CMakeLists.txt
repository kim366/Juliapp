cmake_minimum_required (VERSION 3.8)
project(julia-cpp LANGUAGES CXX)

# C++ settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_CLANG_TIDY clang-tidy)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -fPIC)
endif()

# Julia
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libcxxwrap-julia)
set(Julia_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/julia)
find_package(Julia)

include_directories(src ${Julia_INCLUDE_DIRS})
link_libraries(${Julia_LIBRARY})

# Juliapp target
add_library(juliapp SHARED src/value.cpp src/symbol.cpp src/init.cpp src/module.cpp src/eval.cpp src/global.cpp)
link_libraries(juliapp)

# Demo target
add_executable(demo demo/demo.cpp)

# Catch2
set(Catch2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2/dist/usr/local/lib/cmake/Catch2)
find_package(Catch2 3 REQUIRED CONFIG)

# Tests target
add_executable(tests tests/symbol.cpp tests/module.cpp tests/value.cpp tests/boxing.cpp tests/global.cpp tests/eval.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
