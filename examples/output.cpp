#include <iostream>
#include <print>

import casyyy.maths.numeric_any;
using namespace casyyy::maths;

int main() {
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
    return 0;
}