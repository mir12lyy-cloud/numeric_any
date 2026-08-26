#include <print>

import cy.maths.numeric_any;
using namespace cy::maths;

int main() {
    numeric_any v = -42;
    numeric_any u = 12;
    std::println("{}", v.is_same_type<int>());
    std::println("{}", v.is_same_type<long>());
    std::println("{}", v.can_safe_convert_to<long long>());
    std::println("{}", u.can_safe_convert_to<unsigned long long>());
    std::println("{}", v.can_safe_convert_to<unsigned>());
    std::println("{}", v.can_safe_convert_to<short>());
    std::println("{}", v.can_safe_convert_to<float>());
    return 0;
}
