# numeric_any

![license](https://img.shields.io/badge/license-MIT-blue)
![C++](https://img.shields.io/badge/C%2B%2B-20%2F23-blue)


> A type-erasure-based utility class for arithmetic types. Support addition, subtraction, multiplication, division and comparison.

## Features

- Supports any arithmetic type that conforms to the C++ standard, except `char` (which requires an explicit `signed` or `unsigned` qualification).

- Supports the four arithmetic operations (addition, subtraction, multiplication, division) and comparison operations.

- Supports simple print/output (formatting control is not currently supported).

- No RTTI, no virtual functions, no exceptions, and `constexpr`-friendly.

- Policy-based numeric conversion.

- Based on C++20/23.

- Header-only, provided in both `.ixx` and `.hpp` build variants.

## Installation & Integration

### Environment Requirements
- C++ Standard: At least C++20; C++23 is recommended for a better experience.

- Compiler: At least GCC 13, Clang 17, or MSVC 19.30 (Visual Studio 2019 16.10) or later.

- Operating System: Windows / Linux

- Dependencies: No external dependencies; uses only the C++20 standard library.

**Note:** If you want to use modules to build, Recommend GCC15.

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
- `<format>` – Specialization of std::formatter
- `<charconv>` - For parser transforms numbers to strings

### Clone The depository

```bash
git clone https://github.com/mir12lyy-cloud/numeric_any.git
cd numeric_any
```

### Methods

**Two integration methods are provided: based on the `.hpp` header file, or based on the `.ixx` module. You may choose only one of them.**

**Method 1: Copy `.hpp` directly:**  
Simply copy `numeric_any.hpp`, `numeric_any_parser.hpp` from the root directory of this project into your project.

**Method 2: Introduce `.hpp` via CMake (requires at least CMake 3.30):**  
```cmake
cmake_minimum_required(VERSION 3.30)
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

**Method 3: Introduce `.ixx` via CMake (requires at least CMake 3.30):**  
```cmake
cmake_minimum_required(VERSION 3.30)
project(MyProject)
set(CMAKE_CXX_STANDARD 23) # Or 20.
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
Download `numeric_any.ixx`, `numeric_any_parser.ixx` from the root directory of this project, then build according to your own build tools. A simple CMake build example:

```cmake
target_sources(YourProject
    PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS xxx
        FILES
        numeric_any_parser.ixx 
        numeric_any.ixx
)
```

### Quick Start
```C++
#include <print>

import casyyy.maths.numeric_any;
using namespace casyyy::maths;

int main() {
    numeric_any a = 42;
    std::println("{:03d}", a);
    a = 31.1653;
    std::println("{:.2f}", a);
    return 0;
}
```

### Ouput
```text
042
31.16
```

## Example:

**Ignore basic `import`, `include` and `using namespace`.**

### Construction
**Code:** [Example Code](/examples/construction.cpp)
```C++
numeric_any a;
numeric_any b = 32;
std::println("{} {}", a, b);
a = 32;
b.reset<unsigned char>('a');
std::println("{} {}", a, b);
```
**Output:**
```text
empty 32
32 97
```

### Heterogeneous Container
**Code:** [Example Code](/examples/heterogeneous_container.cpp)
```C++
auto process(auto&&... args) {
    std::vector<numeric_any> result;
    ((result.emplace_back(args)), ...);
    return result;
}

int main() {
    auto result = process(12, 33U, false, 3.14);
    std::println("{}", result);
    return 0;
}
```
**Output:**
```test
[12, 33, false, 3.14]
```
### Metadatas
**Code:** [Example Code](/examples/metadata.cpp)
```C++
numeric_any a;
numeric_any b = 32;
numeric_any c = 12U;
numeric_any d = -32.2;
std::println("{} {} {} {}", a.type_name(), a.is_floating_point(), a.is_unsigned_number(), a.is_nonnegative());
std::println("{} {} {} {}", b.type_name(), b.is_floating_point(), b.is_unsigned_number(), b.is_nonnegative());
std::println("{} {} {} {}", c.type_name(), c.is_floating_point(), c.is_unsigned_number(), c.is_nonnegative());
std::println("{} {} {} {}", d.type_name(), d.is_floating_point(), d.is_unsigned_number(), d.is_nonnegative());
```
**Output:**
```text
empty false false false
int false false true
unsigned int false true true
double true false false
```
### Operations:
**Code:** [Example Code](/examples/operations.cpp)
```C++
numeric_any a = 13;
std::println("{}", a);
std::println("{}", ++a);
std::println("{}", a += 12.3);
std::println("{}", -a);
std::println("{}", a > -a);
std::println("{}", a.type_name());
a = -3;
std::println("{}", a > 12U);
a = true;
std::println("{}", ++a);
a = true;
std::println("{}", --a);
```
**Output:**
```text
13
14
26.3
-26.3
true
double
false
true
false
```
### Type Lookup:
**Code:** [Example Code](/examples/type_lookup.cpp)
```C++
numeric_any v = -42;
numeric_any u = 12;
std::println("{}", v.is_same_type<int>());
std::println("{}", v.is_same_type<long>());
std::println("{}", v.can_safe_convert_to<long long>());
std::println("{}", u.can_safe_convert_to<unsigned long long>());
std::println("{}", v.can_safe_convert_to<unsigned>());
std::println("{}", v.can_safe_convert_to<short>());
std::println("{}", v.can_safe_convert_to<float>());
```
**Output:** 
```text
true
false
true
true
false
false
false
```
### Bytes view:
**Code:** [Example Code](/examples/bytes_view.cpp)
```C++
numeric_any a = 123U;
std::println("{::02x}", a.view_bytes());
 a = 14123.223;
std::println("{::02x}", a.view_bytes());
```
**Possible Output(Based on platform):**
```text
[7b, 00, 00, 00]
[81, 95, 43, 8b, 9c, 95, cb, 40]
```
### Casting:
**Code:** [Example Code](/examples/casting.cpp)
```C++
template <typename T, casting_policy Policy = casting_policy::strict>
void check_casting(const numeric_any& x) {
    auto result = numeric_cast<T, Policy>(x);
    if (result.has_value()) {
        std::println("{}", result.value());
    } else {
        std::println("std::nullopt");
    }
}
int main() {
    numeric_any a = 12312;         // int(12312)
    numeric_any b;                 // Empty.
    numeric_any c = std::nan("1"); // NaN
    check_casting<int>(a);
    check_casting<double>(a);
    check_casting<long long>(a);
    check_casting<int>(b);
    check_casting<double>(c);
    check_casting<float, casting_policy::normal>(a);
    std::println("{}", unchecked_numeric_cast<double>(b));
    std::println("{}", unchecked_numeric_cast<double>(c));
    return 0;
}
```
**Output:**
```text
12312
std::nullopt
12312
std::nullopt
std::nullopt
12312
0
nan
```

### Output
**Code:** [Example Code](/examples/output.cpp)
```C++
numeric_any a = 323423, b = 32342.123;
std::cout << a << ' ' << b << '\n';
std::wcout << a << ' ' << b << '\n';

std::println("{} {}", a, b);
std::println("{:#08x} {:#08x}", a, 323423);
std::println("{:*<6.2f} {:*<6.2f}", b, 32342.123);
std::println("{:0{}d}", a, 8);
std::println("{:0{}d}", 323423, 8);
std::println("{:{}.{}f}", b, 8, 2);
std::println("{:{}.{}f}", 32342.123, 8, 2);
```
**Output:**
```text
323423 32342.1
323423 32342.1
323423 32342.123
0x04ef5f 0x04ef5f
32342.12 32342.12
00323423
00323423
32342.12
32342.12
```

- [Overall example files](/examples/) - Sample code is provided here for you to try and verify on your own.

## API

For the online API documentation, please visit：[https://mir12lyy-cloud.github.io/numeric_any/html/index.html](https://mir12lyy-cloud.github.io/numeric_any/html/index.html)

## Tests (Using Agent to generate)

- [Unit test files](/tests/units/) - You can use or modify these test files to check if there are any issues with the library.

- [Benchmark result](/tests/benchmarks/BENCHMARK.md) - Overview of the result of benchmarks.

- [Benchmark charts](/tests/benchmarks/charts/) - Charts of the result of benchmarks.

- [Benchmark test files](/tests/benchmarks/) - You can use or modify these test files to do basic benchmarks with the library.


## Roadmap

- [x] Support parsing format string in `std::formatter`.
- [ ] Support math functions.
- [ ] More casting policy.

## Contributions
Welcome contributions of any kind. Whether it's submitting an issue to report a bug or forking the project to send a PR. If you find this project helpful, please click the star in the top right to support. Thanks!

## Contact to Me

- Email: huangjinyangyang@hotmail.com

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


**Built with curiosity and caffeine.**


