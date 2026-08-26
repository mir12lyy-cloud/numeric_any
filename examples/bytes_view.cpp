#include <print>

import cy.maths.numeric_any;
using namespace cy::maths;

int main() {
    numeric_any a = 123U;
    std::println("{::02x}", a.view_bytes());
    a = 14123.223;
    std::println("{::02x}", a.view_bytes());
    return 0;
}