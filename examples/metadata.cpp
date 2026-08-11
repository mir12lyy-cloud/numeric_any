#include <print>

import casyyy.maths.numeric_any;
using namespace casyyy::maths;

int main() {
    numeric_any a;
    numeric_any b = 32;
    numeric_any c = 12U;
    numeric_any d = -32.2;

    std::println("{} {} {} {}", a.type_name(), a.is_floating_point(), a.is_unsigned_number(), a.is_nonnegative());
    std::println("{} {} {} {}", b.type_name(), b.is_floating_point(), b.is_unsigned_number(), b.is_nonnegative());
    std::println("{} {} {} {}", c.type_name(), c.is_floating_point(), c.is_unsigned_number(), c.is_nonnegative());
    std::println("{} {} {} {}", d.type_name(), d.is_floating_point(), d.is_unsigned_number(), d.is_nonnegative());
    return 0;
}
