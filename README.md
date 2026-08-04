# numeric_any

![license](https://img.shields.io/badge/license-MIT-blue)
![C++](https://img.shields.io/badge/C%2B%2B-20%2F23-blue)


> A type-erasure-based utility class for arithmetic types. Support addition, subtraction, multiplication, division and comparison.

---

## Features

- Supports any arithmetic type that conforms to the C++ standard, except `char` (which requires an explicit `signed` or `unsigned` qualification).

- Supports the four arithmetic operations (addition, subtraction, multiplication, division) and comparison operations.

- Supports simple print/output (formatting control is not currently supported).

- No RTTI, no virtual functions, no exceptions, and `constexpr`-friendly.

- Policy-based numeric conversion.

- Based on C++20/23.

- Header-only, single-header class, provided in both `.ixx` and `.hpp` build variants.

---

## Quick Start

### Environment Requirements
- C++ Standard: At least C++20; C++23 is recommended for a better experience.

- Compiler: At least GCC 13, Clang 17, or MSVC 19.30 (Visual Studio 2019 16.10) or later.

- Operating System: Windows / Linux

- Dependencies: No external dependencies; uses only the C++20 standard library.

### Standard Library Dependencies
- `<array>` – Static array storage
- `<bit>` – Bit manipulation functions
- `<cmath>` – For checking whether a floating-point number is normal
- `<compare>` – Three-way comparison operator support
- `<cstring>` – Type punning
- `<functional>` – Hash specialization
- `<optional>` – For returning whether a numeric conversion succeeded
- `<ostream>` – Stream output support
- `<span>` – For returning the byte representation of internal values
- `<string_view>` – For providing type name strings
- `<type_traits>` – Type traits
- `<format>` – Specialization of std::formatter (optional).

If you are using the `.hpp` version and wish to avoid the compilation overhead of `<format>`, you can define the `DISABLE_FORMAT_IN_NUMERIC_ANY` macro to exclude `<format>` from compilation.

### Installation & Integration

**Two integration methods are provided: based on the `.hpp` header file, or based on the `.ixx` module. You may choose only one of them.**

**Method 1: Copy `.hpp` directly:**  
Simply copy `numeric_any.hpp` from the root directory of this project into your project.

**Method 2: Introduce `.hpp` via CMake (requires at least CMake 3.20):**  
```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject)
include(FetchContent)
FetchContent_Declare(
  numeric_any_for_hpp
  GIT_REPOSITORY https://github.com/mir12lyy-cloud/numeric_any.git
  GIT_TAG master
)
FetchContent_MakeAvailable(numeric_any_for_hpp)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE numeric_any_for_hpp)
```

**Method 3: Introduce `.ixx` via CMake (requires at least CMake 4.0):**  
```cmake
cmake_minimum_required(VERSION 4.0)
project(MyProject)
include(FetchContent)
FetchContent_Declare(
  numeric_any
  GIT_REPOSITORY https://github.com/mir12lyy-cloud/numeric_any.git
  GIT_TAG master
)
FetchContent_MakeAvailable(numeric_any)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE numeric_any)
```

**Method 4: Download `.ixx` and build manually:**  
Download `numeric_any.ixx` from the root directory of this project, then build according to your own build tools. A simple CMake build example:

```cmake
target_sources(YourProject
    PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS xxx
        FILES numeric_any.ixx
)
```

### Example:

```C++
#include "numeric_any/numeric_any.hpp"
#include <vector>
#include <iostream>
#include <format>
using casyyy::maths::numeric_any;

auto process(auto&& ...args) {
    std::vector<numeric_any> result;
    ((result.emplace_back(args)), ...);
    return result;
}

int main() {
    auto result = process(12, 33U, false, 3.14);
    for (const auto& i : result) {
        std::cout << std::format("{}", i) << ' ';
    }
    return 0;
}
```
### Output：
```
12 33 false 3.14
```
---

## API

To be simple, all examples are seemed to has `using namespace casyyy::maths`. 

### Support types

- Bool - `bool`.
- Signed integers - `signed char`, `short`, `int`, `long`, `long long`.
- Unsigned integers - `unsigned char`, `unsigned short`, `unsigned int`, `unsigned long`, `unsigned long long`.
- Floating points - `float`, `double`, `long double`.

**Note:** To avoid symbol ambiguity in `char`, explict `signed`/`unsigned` is necessary.

### Construction and Storage

- `numeric_any()` / `numreic_any(T x)` - Construct a empty/assigned `numeric_any`.

- `reset<T>(T x)` / `operator=(T x)` - Reset a `numeric_any` to given value. **Type info will change.**

- Copy constructor, move constructor, copy assignment and move assignment keep default.

#### Example:

```C++
numeric_any a;               // Creat a empty numeric_any.
numreic_any b = 32;          // Storage as int(32). 

a = 32;                      // Now a storage as int(32).
b.reset<unsigned char>('a'); // Now b storage as unsigned char('a') -> 97.
```

### Type metadata:

- `type_name()` - Returns the typename of the stored type. **Return `"empty"` when this `numeric_any` is empty.**

- `is_floating_point()` / `is_unsigend_number` / `is_nonegative()` - Type attribute lookup. **Return `false` when this `numeric_any` is empty.**

#### Example

```C++
numeric_any a;
numeric_any b = 32;
numeric_any c = 12U;
numeric_any d = -32.2;

std::cout << std::boolalpha;
std::cout << a.type_name() << ' ' << a.is_floating_point() << ' ' 
          << a.is_unsigend_number() << ' ' << a.is_nonegative() << '\n';
std::cout << b.type_name() << ' ' << b.is_floating_point() << ' ' 
          << b.is_unsigend_number() << ' ' << b.is_nonegative() << '\n';
std::cout << c.type_name() << ' ' << c.is_floating_point() << ' ' 
          << c.is_unsigend_number() << ' ' << c.is_nonegative() << '\n';
std::cout << d.type_name() << ' ' << d.is_floating_point() << ' ' 
          << d.is_unsigend_number() << ' ' << d.is_nonegative() << '\n';
```
#### Output
```text
empty false false false
int false false true
unsigned int false true true
double true false false
```
**Note:** When `numeric_any` storage NaN, `is_nonegative()` returning `false`.

### Type lookup

- `is_same_type<T>()` - Checks if the given type `T` matches the underlying stored type.

- `can_safe_convert_to<T>()` - Checks whether the stored value can be `losslessly` converted to another arithmetic type **without performing the conversion**. 

#### Example:
```C++
numeric_any v = -42;
numeric_any u = 12;
v.is_same_type<int>();                       // true
v.is_same_type<long>();                      // false

v.can_safe_convert_to<long long>();          // true (Promotion)
u.can_safe_convert_to<unsigned long long>(); // true (No negative number)
v.can_safe_convert_to<unsigned>();           // false (Negative number)
v.can_safe_convert_to<short>();              // false (Narrow casting)
v.can_safe_convert_to<float>();              // false (Might lossing precision)
```

**Note:** The `can_safe_convert_to<T>()` won't **do conversion and is conservative**. So all narrow-casting will return `false` whatever the inner value is.

### Operations

- `operator+=`, `operator-=`, `operator*=`, `operator/=` - Support with a value or `numeric_any`. **Behaviours in operatrions are based on C++ standard.**

- `operator<=>`, `opeartor==` - Support with a value or `numeric_any`. **Behaviours mostly in operatrions are based on C++ standard.**

- `operator bool` - Explictly check inner value isn't zero.

#### Example
```C++
numeric_any a = 13;    // a is int(13);
a += 12.3;             // Same as "13 + 12.3" in C++;
numeric_any b = -32;   // b is int(-32);
bool c = a > b;        // Same as "(13 + 12.3) > -32" in C++;
auto d = a.type_name() // "double"
```

**Note:** It will check the sign in comparison, so:

```C++
numreic_any a = -3;
bool b = a > 32U;   // false.
```

**About empty:** If either operand is empty, **propagates the empty state in `operator+=`, `operator-=`, `operator*=`, `operator/=`**. Return `false` in `operator bool`, and empty == empty returns `false`, return `std::partial_ordering::unordered` in `operator<=>`.

### Byte view

- `view_bytes()` - Return a `std::span<const unsigned char>` to view inner bytes.

- `type_size()` - Returns the size in bytes of the stored type.

#### Example 

```C++
numeric_any a = 123U;
std::cout << a.type_size() << "\n";
for (auto i : a.view_bytes()) { 
    std::cout << std::hex << static_cast<unsigned>(i) << ' ';
}
```

### Possible output (Base on platform)
```text
4
7b 0 0 0
```

### Casting Policy (enum class `casting_policy`)
> A simple enum class for policy in casting. Has three policies.
- `strict` - Requires `lossless` numeric conversion. Base on `can_safe_convert_to<T>()`.
- `normal` - Allows integer-to-floating-point conversions, allows signed positive values and unsigned narrowing, and truncation to an unsigned type.
- `relaxed`- Only checks that `numeric_any` is not empty, and that the converted value is not NaN or Inf.


**`strict` is default policy.**

### Casting(Free functions)
- `numreic_cast<T, Policy>(x)` - Casting a `numeric_any` to number based on policy. Return `std::optional<T>`
- `uncheck_numeric_cast<T>(x)` - Casting a `numeric_any` to number without checked.

**Note:** Always return `std::nullopt` when it storages NaN, inf or empty in `numreic_cast`. Return `T{}` in `unchecked_numreic_cast` when empty.

#### Example
```C++
numeric_any a = 12312;         // int(12312)
numeric_any b;                 // Empty.
numeric_any c = std::nan("1"); // NaN

auto d = numeric_cast<int>(a); // Return std::optional<int>(12312);
auto e = numeric_cast<double>(a); // Return std::nullopt.
auto f = numreic_cast<long long>(a); // Return Return std::optional<long long>(12312);
auto g = numreic_cast<int>(b) // Return std::nullopt.
auto h = numreic_cast<double>(c) // Return std::nullopt.
auto i = numeric_cast<float, casting_policy::normal>(a) // Ok.
auto j = unchecked_numeric_cast<double>(c) // Return NaN.
auto k = unchecked_numeric_cast<double>(b) // Return double(0.0);
```

### Output 

- `operator<<` - Output the inner number.

- `std::formatter` - Could used `std::format`, `std::print` and `std::println`.

**Limits:** `std::formatter` can't parse format string.

---
## Tests (Using Agent to generate)

- [Unit test files](/tests/result_numeric_any/) - You can use or modify these test files to check if there are any issues with the library.

- [Benchmark result](/tests/benchmarks/BENCHMARK.md) - Overview of the result of benchmarks.

- [Benchmark charts](/tests/benchmarks/charts/) - Charts of the result of benchmarks.

- [Benchmark test files](/tests/benchmarks/) - You can use or modify these test files to do basic benchmarks with the library.

---

## Roadmap

- [ ] Support parsing format string in `std::formatter`.
- [ ] Support math functions.
- [ ] More casting policy.

## Contributions
Welcome contributions of any kind. Whether it's submitting an issue to report a bug or forking the project to send a PR. If you find this project helpful, please click the star in the top right to support. Thanks!

## Contact to Me

- Email: huangjinyangyang@hotmail.com

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


**Built with curiosity and caffeine.**


