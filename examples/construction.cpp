#include <print>

import casyyy.maths.numeric_any;
using namespace casyyy::maths;

int main() {
    numeric_any a;
    numeric_any b = 32;
    std::println("{} {}", a, b);
    a = 32;
    b.reset<unsigned char>('a');
    std::println("{} {}", a, b);
    return 0;
}
