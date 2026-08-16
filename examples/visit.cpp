#include <cmath>
#include <print>

import casyyy.maths.numeric_any;
using namespace casyyy::maths;

int main() {
    numeric_any a = 32, b{};
    visit(a, [&a](auto i) { a = ::sin(i); }, [] {});
    visit(b, [&b](auto i) { b = ::cos(i); }, [] {});
    std::println("{:.2f} {}", a, b);
}
