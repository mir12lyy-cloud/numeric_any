#include <print>

import casyyy.maths.numeric_any;
using namespace casyyy::maths;

int main() {
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
}