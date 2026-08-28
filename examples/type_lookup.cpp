#include <print>

import cy.maths.numeric_any;
using namespace cy::maths;

int main() {
    numeric_any v = -42;
    numeric_any u = 12;
    std::println("{}", v.is_same_type<int>());
    std::println("{}", v.is_same_type<long>());
    return 0;
}
