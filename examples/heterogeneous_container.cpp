#include <print>
#include <vector>

import cy.maths.numeric_any;
using cy::maths::numeric_any;

auto process(auto&&... args) {
    std::vector<numeric_any> result;
    ((result.emplace_back(args)), ...);
    return result;
}

int main() {
    auto result = process(12, 33U, false, 3.14);
    for (const auto& item : result) {
        std::print("{} ", item);
    }
    return 0;
}