cmake_minimum_required (VERSION 3.8)
project(julia-cpp LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libcxxwrap-julia)
set(Catch2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2/dist/usr/local/lib/cmake/Catch2)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set(Julia_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/julia)
find_package(Julia)
find_package(Catch2 3 REQUIRED CONFIG)

include_directories(src ${Julia_INCLUDE_DIRS})
link_libraries(${Julia_LIBRARIES})

add_executable(demo demo/demo.cpp)

add_executable(tests tests/tests.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
