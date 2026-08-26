#include <cmath>
#include <print>

import cy.maths.numeric_any;
using namespace cy::maths;

int main() {
    numeric_any a = 32;
    visit([&a](auto i) { a = ::sin(i); }, a);
    std::println("{:.2f}", a);
}
