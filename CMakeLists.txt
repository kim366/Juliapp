cmake_minimum_required (VERSION 3.13)
project(julia-cpp LANGUAGES CXX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

option(ENABLE_TESTS "Enable unit tests" ON)
option(ENABLE_COVERAGE "Enable code coverage for unit tests" OFF)
option(DOWNLOAD_LOCAL_JULIA "Download a local Julia installation rather than using the Julia version installed on the system" OFF)

# C++ settings
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -fPIC)
endif()

# Julia
if(DOWNLOAD_LOCAL_JULIA)
    cmake_policy(SET CMP0135 NEW)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        FetchContent_Declare(
                julia
                URL https://julialang-s3.julialang.org/bin/linux/x64/1.8/julia-1.8.5-linux-x86_64.tar.gz
                URL_HASH SHA256=e71a24816e8fe9d5f4807664cbbb42738f5aa9fe05397d35c81d4c5d649b9d05
        )
    else()
        FetchContent_Declare(
                julia
                URL https://julialang-s3.julialang.org/bin/linux/x86/1.8/julia-1.8.5-linux-i686.tar.gz
                URL_HASH SHA256=f0edd61970710333cb5ac6491fbbc859436e5e9e84b014ae04f291bddf6a7e21
        )
    endif()
    FetchContent_MakeAvailable(julia)
    set(Julia_PREFIX ${julia_SOURCE_DIR})
endif()
find_package(Julia REQUIRED)

include_directories(src ${Julia_INCLUDE_DIRS})
link_libraries(${Julia_LIBRARY})

# Juliapp target
add_library(juliapp SHARED src/value.cpp src/symbol.cpp src/init.cpp src/module.cpp src/eval.cpp src/global.cpp src/errors.cpp)
link_libraries(juliapp)
set_target_properties(juliapp PROPERTIES CMAKE_CXX_CLANG_TIDY clang-tidy)

# Coverage
if(ENABLE_COVERAGE)
    target_compile_options(juliapp PRIVATE --coverage -O0)
    target_link_libraries(juliapp PRIVATE gcov)
    add_custom_command(
            TARGET juliapp
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND lcov --capture --directory CMakeFiles/juliapp.dir/src --include "*/src/*" --output-file coverage.info
            COMMAND bash -c "genhtml coverage.info --branch-coverage --output-directory coverage-report | tee coverage-meta.txt"
    )
endif()

# Demo target
add_executable(demo demo/demo.cpp)

if(ENABLE_TESTS)
    # Catch2
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.2.1
            OVERRIDE_FIND_PACKAGE
    )
    find_package(Catch2 3 REQUIRED CONFIG)

    # Tests target
    add_executable(tests tests/symbol.cpp tests/module.cpp tests/value.cpp tests/boxing.cpp tests/global.cpp tests/eval.cpp)
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

    if(ENABLE_COVERAGE)
        target_compile_options(tests PRIVATE --coverage -O0)
        target_link_libraries(tests PRIVATE gcov)
    endif()
endif()
