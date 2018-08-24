# Juliapp

This single-include header-only API wrapper for embedding Julia code within C++ was made with ease-of-use in mind with automatic type deduction, but still keeping explicitness where appropriate.

The API is procedural, just like the official C-API, so you can just execute lines of code and the results will be remembered in a global state instead of per instance.

## Example

Error handling was left out of this example. Check `src/demo.cpp` or the API documentation below to find out more about possible exceptions.

```cpp
#include "Julia.hpp"

int main()
{
    jl::init();

    // Execute a string of Julia code.
    jl::exec("println(\"Hello World!\")");

    // Execute a code withing a multi-line raw string.
    jl::exec(R"(
      module Mod
        const a = 6.
        f(x) = a * x^3
      end
    )");

    // Call the previously defined function with a parameter.
    double res{jl::call("Mod.f", 2.)};

    // Print the returned value.
    std::printf("%f\n", res);

    // Or alternatively, print with Julia.
    jl::call("println", res);

    // Unicode characters are supported.
    jl::exec("println(5 ÷ 2)");

    // Load scripts at runtime! No recompiling of C++ is necessary.
    jl::exec_from_file("sample-script.jl");

    // In Julia, operators are functions. Explicitly cast
    // from `double` to `float`. 
    std::printf("%f\n", jl::call("+", 5, 2, 3., 2.f).get<float>());

    // Declare and initialize a Julia array.
    jl::array<int> arr{5, 8, 1};

    // Pass this array into a Julia function and get another
    // one as a result.
    jl::array<int> reversed_arr{jl::call("reverse", arr)};

    // Or directly manipulate the original array.
    jl::call("reverse!", arr);

    // Raise errors in Julia with printf-formatting
    if (arr != reversed_arr)
      jl::raise_error("Something went wrong! %f", 3.14);

    // Create struct equivalents in Julia and C++. Julia's `struct`s and
    // `NTuple`s are compatible with C++'s stack arrays and `struct`s. The
    // order of the declared types have to be the same.
    struct S
    {
      struct
      {
          float x, y;
      } v;
      // or float v[2];
      
      std::int64_t y;
      float f;
    };

    S& s = jl::exec(R"(
      mutable struct S
        x::NTuple{2, Float32}
        y::Int64
        f::Float32
      end

      s = S((1234, 5678), 88235, 3.847)
    )").get<S&>();

    s.v.x = 5;
    jl::exec("println(s)");

    jl::quit();
}
```
## Keep in mind

* For now only one-dimensional arrays are implemented.
* Cast result values to their desired type in order not to cause any problems to the garbage collector.
* Passing around string values is not supported.
* :warning: Using structs is dangerous. Do not trust user code! Bytes are merely being interpreted and the types must be compatible. Zero validity checking is done! :warning:
* Requesting references to results instead of copies has to be specifically done with `.get<T>()`.
* If you have multiple statements in a `jl::exec` call, the return value will be the result of the last statement.

## API Documentation

### Exceptions

In *Juliapp* two forms of error checking are used: `assert` and exceptions. Assertions are only used where there has been a programming mistake, while exceptions can also occur by changing Julia scripts without changing any C++ code.

#### `jl::error`
This error is never thrown, but is a base for all other exception types and can thus be caught to handle any Julia-related errors.

#### `jl::result_type_error` 
This is thrown whenever an incorrect type is requested out of a Julia script, for example if a script returns an integral type but a floating point type was requested. Note that this error is *not* thrown when e.g. `float` was requested, but `double` was provided.

#### `jl::language_error`
This error is thrown whenever there was an issue in Julia code. In short: it occurs any time a red message would appear in the Julia REPL. `jl::language_error::what()` contains the error name, i.e. one of[Julia - Built-in Exceptions](https://docs.julialang.org/en/stable/manual/control-flow/#Built-in-Exceptions-1) or a custom error.

#### `jl::load_error`
This is thrown when a script failed to load. This may be due to the file name being mistyped or the executable being run from the wrong directory.

### Classes

#### `jl::value`
This is a wrapper around result values. It supports implicit casting (with appropriate compiler warnings if they are enabled) and explicit casting with `jl::value::get<T>()` (see example above). The conversion methods may throw `jl::value_error`.

#### `jl::array<T>`
This type is used for communicating with Julia in terms of arrays. Is is usable like an STL container.

### Functions
All functions accepting `const char*` also accept `const std::string&`.

#### `void jl::init()`
Initialize Julia. Your program will segfault if you do not call this function. Call it only once at the beginning of your program.

#### `void jl::quit(int exit_code = 0)`
Quit Julia. Call it only once at the ed of your program, for example right before returning from `main`. An optional exit code argument may be specified signaling program failure.

#### `jl::value jl::exec(const char* source_string)`
Executes a string of Julia code. Returns the wrapped result value. May throw `jl::result_type_error` or `jl::language_error`.

#### `jl::value jl::exec_from_file(const char* file_name)`
Load the file with the given file name and execute the code within. Note that this path is relative to the current working directory when running the executable. May throw `jl::result_type_error`, `jl::language_error` or `jl::load_error`.

#### `jl::value call(const char* function_name, T... args)`
Call a function denoted by `function_name` with the arguments provided. The function name may also contain a module name (see code above) or other manipulators, as long as it resolves to a function in the REPL. May throw `jl::result_type_error` or `jl::language_error`.

#### `void raise_error(const char* content, T... args)`
Raise an error within Julia. You can use printf-style formatting.
