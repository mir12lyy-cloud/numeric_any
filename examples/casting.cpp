#include <cmath>
#include <print>

import cy.maths.numeric_any;
using namespace cy::maths;

template <typename T, casting_policy Policy = casting_policy::strict>
void check_casting(const numeric_any& x) {
    auto result = from<T, Policy>(x);
    if (result.has_value()) {
        std::println("{}", result.value());
    } else {
        std::println("std::nullopt");
    }
}

int main() {
    numeric_any a = 12312;         // int(12312)
    numeric_any c = std::nan("1"); // NaN

    check_casting<int>(a);
    check_casting<double>(a);
    check_casting<long long>(a);
    check_casting<double>(c);
    check_casting<float, casting_policy::normal>(a);
    std::println("{}", as<double>(c));
    return 0;
}