#include <print>
#include <vector>

import casyyy.maths.numeric_any;
using casyyy::maths::numeric_any;

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