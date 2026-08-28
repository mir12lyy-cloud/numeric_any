#include <print>

import cy.maths.numeric_any;
using namespace cy::maths;

int main() {
    numeric_any a;
    numeric_any b = 32;
    std::println("{}", b);
    a = 32;
    b.reset<unsigned char>('a');
    std::println("{} {}", a, b);
    return 0;
}
