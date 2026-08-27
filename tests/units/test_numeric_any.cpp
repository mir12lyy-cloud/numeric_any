// Comprehensive tests for cy::maths::numeric_any
// Covers all arithmetic types, edge cases, promotions, and policies.

#include "../../src/numeric_any.hpp"
#include <gtest/gtest.h>

#include <cmath>
#include <format>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

using namespace cy::maths;

// ============================================================================
// 1. Construction Tests
// ============================================================================

TEST(ConstructionTest, BoolConstruction) {
    numeric_any a{true};
    EXPECT_EQ(a.type_name(), "bool");
    EXPECT_TRUE(a.is_same_type<bool>());
    EXPECT_EQ(as<bool>(a), true);

    numeric_any b{false};
    EXPECT_EQ(as<bool>(b), false);
    EXPECT_FALSE(static_cast<bool>(b));
}

TEST(ConstructionTest, SignedCharConstruction) {
    auto sc42 = static_cast<signed char>(42);
    numeric_any a{sc42};
    EXPECT_EQ(a.type_name(), "signed char");
    EXPECT_TRUE(a.is_same_type<signed char>());
    EXPECT_EQ(as<signed char>(a), 42);

    auto sc128 = static_cast<signed char>(-128);
    numeric_any b{sc128};
    EXPECT_EQ(as<signed char>(b), -128);
    EXPECT_FALSE(b.is_nonnegative());
}

TEST(ConstructionTest, UnsignedCharConstruction) {
    auto uc255 = static_cast<unsigned char>(255);
    numeric_any a{uc255};
    EXPECT_EQ(a.type_name(), "unsigned char");
    EXPECT_TRUE(a.is_same_type<unsigned char>());
    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_EQ(as<unsigned char>(a), 255);
}

TEST(ConstructionTest, ShortConstruction) {
    numeric_any a{short{32767}};
    EXPECT_EQ(a.type_name(), "short");
    EXPECT_TRUE(a.is_same_type<short>());
    EXPECT_EQ(as<short>(a), 32767);

    numeric_any b{short{-32768}};
    EXPECT_FALSE(b.is_nonnegative());
}

TEST(ConstructionTest, UnsignedShortConstruction) {
    auto us65535 = static_cast<unsigned short>(65535);
    numeric_any a{us65535};
    EXPECT_EQ(a.type_name(), "unsigned short");
    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_EQ(as<unsigned short>(a), 65535);
}

TEST(ConstructionTest, IntConstruction) {
    numeric_any a{42};
    EXPECT_EQ(a.type_name(), "int");
    EXPECT_TRUE(a.is_same_type<int>());
    EXPECT_EQ(as<int>(a), 42);

    numeric_any b{-100};
    EXPECT_FALSE(b.is_nonnegative());
}

TEST(ConstructionTest, UnsignedIntConstruction) {
    numeric_any a{42U};
    EXPECT_EQ(a.type_name(), "unsigned int");
    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_TRUE(a.is_nonnegative());
    EXPECT_EQ(as<unsigned int>(a), 42U);
}

TEST(ConstructionTest, LongConstruction) {
    numeric_any a{42L};
    EXPECT_EQ(a.type_name(), "long");
    EXPECT_TRUE(a.is_same_type<long>());
    EXPECT_EQ(as<long>(a), 42L);
}

TEST(ConstructionTest, UnsignedLongConstruction) {
    numeric_any a{42UL};
    EXPECT_EQ(a.type_name(), "unsigned long");
    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_EQ(as<unsigned long>(a), 42UL);
}

TEST(ConstructionTest, LongLongConstruction) {
    numeric_any a{42LL};
    EXPECT_EQ(a.type_name(), "long long");
    EXPECT_TRUE(a.is_same_type<long long>());
    EXPECT_EQ(as<long long>(a), 42LL);
}

TEST(ConstructionTest, UnsignedLongLongConstruction) {
    numeric_any a{42ULL};
    EXPECT_EQ(a.type_name(), "unsigned long long");
    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_EQ(as<unsigned long long>(a), 42ULL);
}

TEST(ConstructionTest, FloatConstruction) {
    numeric_any a{3.14f};
    EXPECT_EQ(a.type_name(), "float");
    EXPECT_TRUE(a.is_floating_point());
    EXPECT_TRUE(a.is_same_type<float>());
    EXPECT_FLOAT_EQ(as<float>(a), 3.14f);
}

TEST(ConstructionTest, DoubleConstruction) {
    numeric_any a{3.14159265358979};
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_TRUE(a.is_floating_point());
    EXPECT_TRUE(a.is_same_type<double>());
    EXPECT_DOUBLE_EQ(as<double>(a), 3.14159265358979);
}

TEST(ConstructionTest, LongDoubleConstruction) {
    numeric_any a{3.14159265358979L};
    EXPECT_EQ(a.type_name(), "long double");
    EXPECT_TRUE(a.is_floating_point());
    EXPECT_TRUE(a.is_same_type<long double>());
    EXPECT_DOUBLE_EQ(static_cast<double>(as<long double>(a)), 3.14159265358979);
}

// ============================================================================
// 2. Reset & Assignment Tests
// ============================================================================

TEST(ResetTest, ResetChangesType) {
    numeric_any a{42}; // int
    EXPECT_TRUE(a.is_same_type<int>());

    a.reset(3.14); // double
    EXPECT_TRUE(a.is_same_type<double>());
    EXPECT_DOUBLE_EQ(as<double>(a), 3.14);

    a.reset(100ULL); // unsigned long long
    EXPECT_TRUE(a.is_same_type<unsigned long long>());
    EXPECT_EQ(as<unsigned long long>(a), 100ULL);
}

TEST(ResetTest, OperatorEqualsChangesType) {
    numeric_any a{42};
    a = 3.14f;
    EXPECT_TRUE(a.is_same_type<float>());
    EXPECT_FLOAT_EQ(as<float>(a), 3.14f);

    a = true;
    EXPECT_TRUE(a.is_same_type<bool>());
    EXPECT_EQ(as<bool>(a), true);
}

// ============================================================================
// 3. Type Metadata Tests
// ============================================================================

TEST(TypeMetadataTest, BoolMetadata) {
    numeric_any a{true};
    EXPECT_EQ(a.type_name(), "bool");
    EXPECT_FALSE(a.is_floating_point());

    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_TRUE(a.is_nonnegative());
}

TEST(TypeMetadataTest, SignedIntegerMetadata) {
    numeric_any a{-42};
    EXPECT_EQ(a.type_name(), "int");
    EXPECT_FALSE(a.is_floating_point());
    EXPECT_FALSE(a.is_unsigned_number());
    EXPECT_FALSE(a.is_nonnegative());
}

TEST(TypeMetadataTest, UnsignedIntegerMetadata) {
    numeric_any a{42U};
    EXPECT_EQ(a.type_name(), "unsigned int");
    EXPECT_FALSE(a.is_floating_point());
    EXPECT_TRUE(a.is_unsigned_number());
    EXPECT_TRUE(a.is_nonnegative());
}

TEST(TypeMetadataTest, FloatingPointMetadata) {
    numeric_any a{3.14};
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_TRUE(a.is_floating_point());
    EXPECT_FALSE(a.is_unsigned_number());
    EXPECT_TRUE(a.is_nonnegative());

    numeric_any b{-3.14};
    EXPECT_TRUE(b.is_floating_point());
    EXPECT_FALSE(b.is_nonnegative());
}

// ============================================================================
// 4. Type Lookup Tests (is_same_type / from<T> with default strict policy)
// ============================================================================

TEST(TypeLookupTest, IsSameType) {
    numeric_any a{42};
    EXPECT_TRUE(a.is_same_type<int>());
    EXPECT_FALSE(a.is_same_type<long>());
    EXPECT_FALSE(a.is_same_type<double>());

    numeric_any b{3.14};
    EXPECT_TRUE(b.is_same_type<double>());
    EXPECT_FALSE(b.is_same_type<float>());
}

TEST(TypeLookupTest, StrictBoolConvertibility) {
    numeric_any a{true};
    EXPECT_TRUE(from<bool>(a).has_value());
    EXPECT_TRUE(from<unsigned int>(a).has_value());
    EXPECT_TRUE(from<unsigned long>(a).has_value());
    EXPECT_TRUE(from<unsigned long long>(a).has_value());
    EXPECT_TRUE(from<int>(a).has_value());
    EXPECT_TRUE(from<double>(a).has_value());
    EXPECT_TRUE(from<long double>(a).has_value());
}

TEST(TypeLookupTest, StrictSignedIntegerConvertibility) {
    numeric_any a{42}; // int
    EXPECT_TRUE(from<int>(a).has_value());
    EXPECT_TRUE(from<long>(a).has_value());
    EXPECT_TRUE(from<long long>(a).has_value());
    EXPECT_FALSE(from<short>(a).has_value());
    EXPECT_FALSE(from<signed char>(a).has_value());
    EXPECT_FALSE(from<unsigned int>(a).has_value());
    EXPECT_FALSE(from<unsigned long>(a).has_value());
    EXPECT_FALSE(from<unsigned long long>(a).has_value());
    EXPECT_FALSE(from<float>(a).has_value());
    EXPECT_FALSE(from<double>(a).has_value());
}

TEST(TypeLookupTest, StrictNegativeIntegerConvertibility) {
    numeric_any a{-42};
    EXPECT_TRUE(from<int>(a).has_value());
    EXPECT_FALSE(from<unsigned int>(a).has_value());
    EXPECT_FALSE(from<unsigned long>(a).has_value());
    EXPECT_FALSE(from<unsigned long long>(a).has_value());
}

TEST(TypeLookupTest, StrictUnsignedIntegerConvertibility) {
    numeric_any a{42U};
    EXPECT_TRUE(from<unsigned int>(a).has_value());
    EXPECT_TRUE(from<unsigned long>(a).has_value());
    EXPECT_TRUE(from<unsigned long long>(a).has_value());
    EXPECT_FALSE(from<int>(a).has_value());
    EXPECT_FALSE(from<unsigned short>(a).has_value());
    EXPECT_FALSE(from<float>(a).has_value());
}

TEST(TypeLookupTest, StrictFloatingPointConvertibility) {
    numeric_any a{3.14f};
    EXPECT_TRUE(from<float>(a).has_value());
    EXPECT_TRUE(from<double>(a).has_value());
    EXPECT_TRUE(from<long double>(a).has_value());
    EXPECT_FALSE(from<int>(a).has_value());
    EXPECT_FALSE(from<unsigned int>(a).has_value());
}

// ============================================================================
// 5. Arithmetic Operation Tests (with scalars)
// ============================================================================

TEST(ArithmeticTest, AddWithScalar) {
    numeric_any a{10};
    a += 5;
    EXPECT_EQ(as<int>(a), 15);

    numeric_any b{3.5};
    b += 2.5;
    EXPECT_DOUBLE_EQ(as<double>(b), 6.0);
}

TEST(ArithmeticTest, SubWithScalar) {
    numeric_any a{10};
    a -= 3;
    EXPECT_EQ(as<int>(a), 7);
}

TEST(ArithmeticTest, MulWithScalar) {
    numeric_any a{10};
    a *= 3;
    EXPECT_EQ(as<int>(a), 30);
}

TEST(ArithmeticTest, DivWithScalar) {
    numeric_any a{10};
    a /= 2;
    EXPECT_EQ(as<int>(a), 5);
}

TEST(ArithmeticTest, AddPromotesIntToDouble) {
    numeric_any a{10};
    a += 3.5;
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_DOUBLE_EQ(as<double>(a), 13.5);
}

TEST(ArithmeticTest, AddPromotesFloatToDouble) {
    numeric_any a{3.14f};
    a += 2.718281828;
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_NEAR(as<double>(a), 5.858281828, 1e-6);
}

TEST(ArithmeticTest, AddPromotesIntToLongLong) {
    numeric_any a{100};
    a += 10000000000LL;
    EXPECT_EQ(a.type_name(), "long long");
    EXPECT_EQ(as<long long>(a), 10000000100LL);
}

TEST(ArithmeticTest, AddPromotesUnsignedIntToUnsignedLongLong) {
    numeric_any a{100U};
    a += 10000000000ULL;
    EXPECT_EQ(a.type_name(), "unsigned long long");
    EXPECT_EQ(as<unsigned long long>(a), 10000000100ULL);
}

TEST(ArithmeticTest, AddIntAndUnsignedIntPromotes) {
    numeric_any a{10};
    a += 20U;
    EXPECT_EQ(a.type_name(), "unsigned int");
    EXPECT_EQ(as<unsigned int>(a), 30U);
}

TEST(ArithmeticTest, FreeFunctionOperators) {
    numeric_any a{10};
    numeric_any b{5};

    auto c = a + b;
    EXPECT_EQ(as<int>(c), 15);

    auto d = a - b;
    EXPECT_EQ(as<int>(d), 5);

    auto e = a * b;
    EXPECT_EQ(as<int>(e), 50);

    auto f = a / b;
    EXPECT_EQ(as<int>(f), 2);
}

TEST(ArithmeticTest, FreeFunctionScalarOperators) {
    numeric_any a{10};
    auto b = a + 5;
    EXPECT_EQ(as<int>(b), 15);

    auto c = 5 + a;
    EXPECT_EQ(as<int>(c), 15);
}

// ============================================================================
// 7. Comparison Tests
// ============================================================================

TEST(ComparisonTest, IntEquality) {
    numeric_any a{42};
    EXPECT_TRUE(a == 42);
    EXPECT_FALSE(a == 43);
}

TEST(ComparisonTest, IntThreeWayComparison) {
    numeric_any a{42};
    EXPECT_EQ(a <=> 42, std::partial_ordering::equivalent);
    EXPECT_EQ(a <=> 10, std::partial_ordering::greater);
    EXPECT_EQ(a <=> 100, std::partial_ordering::less);
}

TEST(ComparisonTest, DoubleComparison) {
    numeric_any a{3.14};
    EXPECT_TRUE(a == 3.14);
    EXPECT_EQ(a <=> 3.14, std::partial_ordering::equivalent);
    EXPECT_EQ(a <=> 3.0, std::partial_ordering::greater);
    EXPECT_EQ(a <=> 4.0, std::partial_ordering::less);
}

TEST(ComparisonTest, CrossTypeComparison) {
    numeric_any a{42};   // int
    numeric_any b{42.0}; // double
    EXPECT_TRUE(a == b);
    EXPECT_EQ(a <=> b, std::partial_ordering::equivalent);
}

TEST(ComparisonTest, SignedUnsignedComparison) {
    numeric_any a{-3};
    numeric_any b{32U};
    EXPECT_EQ(a <=> b, std::partial_ordering::less);
}

TEST(ComparisonTest, CrossTypeComparisonIntAndDouble) {
    numeric_any a{42}; // int
    EXPECT_TRUE(a == 42.0);
    EXPECT_EQ(a <=> 42.0, std::partial_ordering::equivalent);
    EXPECT_EQ(a <=> 41.9, std::partial_ordering::greater);
    EXPECT_EQ(a <=> 42.1, std::partial_ordering::less);
}

TEST(ComparisonTest, FloatAndDoubleComparison) {
    numeric_any a{3.14f};
    numeric_any b{3.14};
    EXPECT_NE(a <=> b, std::partial_ordering::equivalent);
}

// ============================================================================
// 8. Operator Bool Tests
// ============================================================================

TEST(OperatorBoolTest, NonZeroIsTrue) {
    numeric_any a{42};
    EXPECT_TRUE(static_cast<bool>(a));

    numeric_any b{0.1};
    EXPECT_TRUE(static_cast<bool>(b));
}

TEST(OperatorBoolTest, ZeroIsFalse) {
    numeric_any a{0};
    EXPECT_FALSE(static_cast<bool>(a));

    numeric_any b{0.0};
    EXPECT_FALSE(static_cast<bool>(b));

    numeric_any c{0U};
    EXPECT_FALSE(static_cast<bool>(c));

    numeric_any d{false};
    EXPECT_FALSE(static_cast<bool>(d));
}

TEST(OperatorBoolTest, NegativeIsTrue) {
    numeric_any a{-1};
    EXPECT_TRUE(static_cast<bool>(a));
}

// ============================================================================
// 9. Floating-Point Edge Cases (NaN, Inf, subnormal)
// ============================================================================

TEST(FloatingPointEdgeTest, NaNConstruction) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_TRUE(std::isnan(as<double>(a)));
}

TEST(FloatingPointEdgeTest, NaNIsNotNonnegative) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    EXPECT_FALSE(a.is_nonnegative());
}

TEST(FloatingPointEdgeTest, NaNComparisonIsUnordered) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    numeric_any b{42.0};
    // NaN compared to anything is unordered
    EXPECT_EQ(a <=> b, std::partial_ordering::unordered);
    EXPECT_EQ(b <=> a, std::partial_ordering::unordered);
    EXPECT_FALSE(a == b);
}

TEST(FloatingPointEdgeTest, NaNEqualsNaNReturnsFalse) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    numeric_any b{nan_val};
    EXPECT_FALSE(a == b);
    EXPECT_EQ(a <=> b, std::partial_ordering::unordered);
}

TEST(FloatingPointEdgeTest, NaNComparisonWithScalar) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    EXPECT_FALSE(a == 0.0);
    EXPECT_FALSE(a == 1.0);
    EXPECT_EQ(a <=> 0.0, std::partial_ordering::unordered);
}

TEST(FloatingPointEdgeTest, ScalarNaNComparison) {
    numeric_any a{42.0};
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    EXPECT_FALSE(a == nan_val);
    EXPECT_EQ(a <=> nan_val, std::partial_ordering::unordered);
}

TEST(FloatingPointEdgeTest, PositiveInfinity) {
    double inf = std::numeric_limits<double>::infinity();
    numeric_any a{inf};
    EXPECT_TRUE(std::isinf(as<double>(a)));

    numeric_any b{42.0};
    EXPECT_EQ(a <=> b, std::partial_ordering::greater);
    EXPECT_EQ(b <=> a, std::partial_ordering::less);
}

TEST(FloatingPointEdgeTest, NegativeInfinity) {
    double neg_inf = -std::numeric_limits<double>::infinity();
    numeric_any a{neg_inf};
    EXPECT_TRUE(std::isinf(as<double>(a)));

    numeric_any b{42.0};
    EXPECT_EQ(a <=> b, std::partial_ordering::less);
}

TEST(FloatingPointEdgeTest, InfOperation) {
    double inf = std::numeric_limits<double>::infinity();
    numeric_any a{inf};
    a += 1.0;
    EXPECT_TRUE(std::isinf(as<double>(a)));

    a *= 2.0;
    EXPECT_TRUE(std::isinf(as<double>(a)));
}

TEST(FloatingPointEdgeTest, NaNOperation) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    a += 1.0;
    EXPECT_TRUE(std::isnan(as<double>(a)));
}

TEST(FloatingPointEdgeTest, SubnormalFloat) {
    float subnormal = std::numeric_limits<float>::denorm_min();
    numeric_any a{subnormal};
    EXPECT_EQ(a.type_name(), "float");
    EXPECT_GT(as<float>(a), 0.0f);
    EXPECT_LT(as<float>(a), std::numeric_limits<float>::min());
}

TEST(FloatingPointEdgeTest, SubnormalDouble) {
    double subnormal = std::numeric_limits<double>::denorm_min();
    numeric_any a{subnormal};
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_GT(as<double>(a), 0.0);
    EXPECT_LT(as<double>(a), std::numeric_limits<double>::min());
}

TEST(FloatingPointEdgeTest, NegativeZero) {
    double neg_zero = -0.0;
    numeric_any a{neg_zero};
    EXPECT_DOUBLE_EQ(as<double>(a), 0.0);
    EXPECT_TRUE(a.is_nonnegative());
}

TEST(FloatingPointEdgeTest, FloatMax) {
    float max_val = std::numeric_limits<float>::max();
    numeric_any a{max_val};
    EXPECT_FLOAT_EQ(as<float>(a), max_val);
}

TEST(FloatingPointEdgeTest, DoubleMax) {
    double max_val = std::numeric_limits<double>::max();
    numeric_any a{max_val};
    EXPECT_DOUBLE_EQ(as<double>(a), max_val);
}

TEST(FloatingPointEdgeTest, FloatMin) {
    float min_val = std::numeric_limits<float>::min();
    numeric_any a{min_val};
    EXPECT_FLOAT_EQ(as<float>(a), min_val);
    EXPECT_GT(as<float>(a), 0.0f);
}

// ============================================================================
// 10. From (checked cast) Tests - policies: equal / strict / normal
// ============================================================================

TEST(FromTest, SameTypeSucceedsUnderAllPolicies) {
    numeric_any a{42};
    EXPECT_TRUE(from<int>(a).has_value());
    EXPECT_TRUE((from<int, casting_policy::equal>(a).has_value()));
    EXPECT_TRUE((from<int, casting_policy::strict>(a).has_value()));
    EXPECT_TRUE((from<int, casting_policy::normal>(a).has_value()));
    EXPECT_EQ(*from<int>(a), 42);
}

TEST(FromTest, StrictPolicyFailureNarrowing) {
    numeric_any a{42}; // int
    EXPECT_FALSE(from<short>(a).has_value());
}

TEST(FromTest, StrictPolicyRejectsSignedToUnsigned) {
    numeric_any a{-42};
    EXPECT_FALSE(from<unsigned int>(a).has_value());
    numeric_any b{42};
    EXPECT_FALSE(from<unsigned long>(b).has_value());
}

TEST(FromTest, StrictPolicyAllowsSameCategoryWidening) {
    numeric_any i{42};
    ASSERT_TRUE(from<long>(i).has_value());
    EXPECT_EQ(*from<long>(i), 42L);
    EXPECT_EQ(*from<long long>(i), 42LL);

    auto sc = static_cast<signed char>(7);
    numeric_any sca{sc};
    EXPECT_EQ(*from<int>(sca), 7);
    EXPECT_EQ(*from<short>(sca), 7);

    numeric_any u{42U};
    EXPECT_EQ(*from<unsigned long>(u), 42UL);
    EXPECT_EQ(*from<unsigned long long>(u), 42ULL);

    numeric_any f{3.14f};
    ASSERT_TRUE(from<double>(f).has_value());
    EXPECT_NEAR(*from<double>(f), 3.14, 1e-6);
    auto ld = from<long double>(f);
    ASSERT_TRUE(ld.has_value());
    EXPECT_NEAR(static_cast<double>(*ld), 3.14, 1e-6);

    numeric_any d{2.5};
    EXPECT_DOUBLE_EQ(static_cast<double>(*from<long double>(d)), 2.5);
}

TEST(FromTest, StrictPolicyRejectsCategoryMismatch) {
    numeric_any i{42};
    EXPECT_FALSE(from<double>(i).has_value());
    EXPECT_FALSE(from<float>(i).has_value());
    EXPECT_FALSE(from<long double>(i).has_value());

    numeric_any f{3.14f};
    EXPECT_FALSE(from<int>(f).has_value());
    EXPECT_FALSE(from<long long>(f).has_value());
    EXPECT_FALSE(from<unsigned int>(f).has_value());
}

TEST(FromTest, StrictPolicyRejectsSignMismatch) {
    numeric_any i{42};
    EXPECT_FALSE(from<unsigned int>(i).has_value());
    EXPECT_FALSE(from<unsigned long>(i).has_value());
    EXPECT_FALSE(from<unsigned long long>(i).has_value());

    numeric_any u{42U};
    EXPECT_FALSE(from<int>(u).has_value());
    EXPECT_FALSE(from<long long>(u).has_value());
}

TEST(FromTest, StrictPolicyRejectsFloatingNarrowing) {
    numeric_any d{3.14};
    EXPECT_FALSE(from<float>(d).has_value());
}

TEST(FromTest, StrictPolicyFailureFloatToInt) {
    numeric_any a{3.14};
    EXPECT_FALSE(from<int>(a).has_value());
}

TEST(FromTest, EqualPolicyRequiresSameType) {
    numeric_any a{42};
    EXPECT_TRUE((from<int, casting_policy::equal>(a).has_value()));
    EXPECT_FALSE((from<long long, casting_policy::equal>(a).has_value()));

    numeric_any b{3.14};
    EXPECT_TRUE((from<double, casting_policy::equal>(b).has_value()));
    EXPECT_FALSE((from<float, casting_policy::equal>(b).has_value()));
}

TEST(FromTest, NormalPolicyAllowsIntToFloat) {
    numeric_any a{42};
    auto res = from<double, casting_policy::normal>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_DOUBLE_EQ(*res, 42.0);
    auto res2 = from<long double, casting_policy::normal>(a);
    ASSERT_TRUE(res2.has_value());
    EXPECT_DOUBLE_EQ(static_cast<double>(*res2), 42.0);
}

TEST(FromTest, NormalPolicyAllowsUnsignedNarrowing) {
    numeric_any a{100UL};
    auto res = from<unsigned int, casting_policy::normal>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, 100U);
}

TEST(FromTest, NormalPolicyTruncatesFloatToInt) {
    numeric_any a{3.14};
    auto res = from<int, casting_policy::normal>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, 3);

    numeric_any b{-3.14};
    auto res2 = from<int, casting_policy::normal>(b);
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(*res2, -3);
}

TEST(FromTest, NormalPolicyRejectsOutOfRange) {
    numeric_any a{100000};
    EXPECT_FALSE((from<short, casting_policy::normal>(a).has_value()));

    numeric_any b{1e300};
    EXPECT_FALSE((from<int, casting_policy::normal>(b).has_value()));

    numeric_any c{-42};
    EXPECT_FALSE((from<unsigned int, casting_policy::normal>(c).has_value()));
}

TEST(FromTest, NormalPolicyAllowsSignedToUnsignedInRange) {
    numeric_any a{42};
    auto res = from<unsigned int, casting_policy::normal>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, 42U);
}

TEST(FromTest, NormalPolicyFloatTargetAcceptsZeroAndNegative) {
    numeric_any zero{0};
    auto r1 = from<float, casting_policy::normal>(zero);
    ASSERT_TRUE(r1.has_value());
    EXPECT_FLOAT_EQ(*r1, 0.0f);
    auto r2 = from<double, casting_policy::normal>(zero);
    ASSERT_TRUE(r2.has_value());
    EXPECT_DOUBLE_EQ(*r2, 0.0);

    numeric_any neg{-5};
    auto r3 = from<float, casting_policy::normal>(neg);
    ASSERT_TRUE(r3.has_value());
    EXPECT_FLOAT_EQ(*r3, -5.0f);
    auto r4 = from<double, casting_policy::normal>(neg);
    ASSERT_TRUE(r4.has_value());
    EXPECT_DOUBLE_EQ(*r4, -5.0);
}

TEST(FromTest, NaNHandling) {
    double nan_val = std::numeric_limits<double>::quiet_NaN();
    numeric_any a{nan_val};
    ASSERT_TRUE(from<double>(a).has_value());
    EXPECT_TRUE(std::isnan(*from<double>(a)));
    ASSERT_TRUE((from<double, casting_policy::normal>(a).has_value()));
    EXPECT_TRUE((std::isnan(*from<double, casting_policy::normal>(a))));
    ASSERT_TRUE(from<long double>(a).has_value());
    EXPECT_TRUE(std::isnan(*from<long double>(a)));
    EXPECT_FALSE(from<float>(a).has_value());
    EXPECT_FALSE((from<float, casting_policy::normal>(a).has_value()));
    EXPECT_FALSE((from<int, casting_policy::normal>(a).has_value()));
    EXPECT_FALSE((from<long double, casting_policy::normal>(a).has_value()));
}

TEST(FromTest, InfinityHandling) {
    double inf = std::numeric_limits<double>::infinity();
    numeric_any a{inf};
    ASSERT_TRUE(from<double>(a).has_value());
    EXPECT_TRUE(std::isinf(*from<double>(a)));
    EXPECT_FALSE((from<float, casting_policy::normal>(a).has_value()));
    EXPECT_FALSE((from<int, casting_policy::normal>(a).has_value()));
    ASSERT_TRUE(from<long double>(a).has_value());
    EXPECT_TRUE(std::isinf(*from<long double>(a)));
}

TEST(FromTest, UncheckedAsTruncates) {
    numeric_any a{3.99};
    EXPECT_EQ(as<int>(a), 3);
    numeric_any b{-3.99};
    EXPECT_EQ(as<int>(b), -3);
}

// ============================================================================
// 11. Byte View Tests
// ============================================================================

TEST(ByteViewTest, ViewBytesInt) {
    numeric_any a{0x12345678};
    auto bytes = a.view_bytes();
    EXPECT_EQ(bytes.size(), sizeof(int));
    EXPECT_EQ(a.type_size(), sizeof(int));
}

TEST(ByteViewTest, ViewBytesDouble) {
    numeric_any a{3.14};
    auto bytes = a.view_bytes();
    EXPECT_EQ(bytes.size(), sizeof(double));
    EXPECT_EQ(a.type_size(), sizeof(double));
}

// ============================================================================
// 12. Output / Stream Tests
// ============================================================================

TEST(OutputStreamTest, OutputInt) {
    numeric_any a{42};
    std::ostringstream oss;
    std::wostringstream oss2;
    oss << a;
    oss2 << a;
    EXPECT_EQ(oss.str(), "42");
    EXPECT_EQ(oss2.str(), L"42");
}

TEST(OutputStreamTest, OutputDouble) {
    numeric_any a{3.14};
    std::ostringstream oss;
    std::wostringstream oss2;
    oss << a;
    oss2 << a;
    // The stream output of 3.14 as double
    EXPECT_NE(oss.str().find("3.14"), std::string::npos);
    EXPECT_NE(oss2.str().find(L"3.14"), std::wstring::npos);
}

TEST(OutputStreamTest, OutputBool) {
    numeric_any a{true};
    std::ostringstream oss;
    std::wostringstream woss;
    oss << a;
    woss << a;
    EXPECT_EQ(oss.str(), "1");
    EXPECT_EQ(woss.str(), L"1");

    numeric_any b{false};
    std::ostringstream oss2;
    std::wostringstream woss2;
    oss2 << b;
    woss2 << b;
    EXPECT_EQ(oss2.str(), "0");
    EXPECT_EQ(woss2.str(), L"0");
}

TEST(OutputStreamTest, OutputNegativeInt) {
    numeric_any a{-42};
    std::ostringstream oss;
    std::wostringstream oss2;
    oss << a;
    oss2 << a;
    EXPECT_EQ(oss.str(), "-42");
    EXPECT_EQ(oss2.str(), L"-42");
}

// ============================================================================
// 13. Hash Tests
// ============================================================================

TEST(HashTest, HashInt) {
    numeric_any a{42};
    std::hash<numeric_any> hasher;
    auto h = hasher(a);
    EXPECT_NE(h, 0u);
}

TEST(HashTest, SameValueSameHash) {
    numeric_any a{42};
    numeric_any b{42};
    std::hash<numeric_any> hasher;
    EXPECT_EQ(hasher(a), hasher(b));
}

// ============================================================================
// 14. Type Promotion According to C++ Standard Tests
// ============================================================================

TEST(CppPromotionTest, SmallerThanIntPromotesToInt) {
    // signed char + signed char -> int
    auto sc10 = static_cast<signed char>(10);
    auto sc20 = static_cast<signed char>(20);
    numeric_any a{sc10};
    a += sc20;
    EXPECT_EQ(a.type_name(), "int");
    EXPECT_EQ(as<int>(a), 30);
}

TEST(CppPromotionTest, UnsignedShortPromotion) {
    auto us100 = static_cast<unsigned short>(100);
    numeric_any a{us100};
    a += 50;
    EXPECT_EQ(as<int>(a), 150);
}

TEST(CppPromotionTest, IntPlusDoubleGivesDouble) {
    numeric_any a{10};
    a += 3.5;
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_DOUBLE_EQ(as<double>(a), 13.5);
}

TEST(CppPromotionTest, IntPlusUnsignedIntGivesUnsignedInt) {
    numeric_any a{10};
    a += 20U;
    EXPECT_EQ(a.type_name(), "unsigned int");
    EXPECT_EQ(as<unsigned int>(a), 30U);
}

TEST(CppPromotionTest, LongDoubleDominates) {
    numeric_any a{10};
    a += 3.14L;
    EXPECT_EQ(a.type_name(), "long double");
    EXPECT_NEAR(static_cast<double>(as<long double>(a)), 13.14, 0.01);
}

TEST(CppPromotionTest, DoubleDominatesFloat) {
    numeric_any a{3.14f};
    a += 2.718281828;
    EXPECT_EQ(a.type_name(), "double");
}

TEST(CppPromotionTest, UnsignedLongLongDominates) {
    numeric_any a{10};
    a += 100ULL;
    EXPECT_EQ(a.type_name(), "unsigned long long");
    EXPECT_EQ(as<unsigned long long>(a), 110ULL);
}

TEST(CppPromotionTest, LongLongDominatesLong) {
    numeric_any a{10L};
    a += 100LL;
    EXPECT_EQ(a.type_name(), "long long");
    EXPECT_EQ(as<long long>(a), 110LL);
}

// ============================================================================
// 15. MakeNumericAny Tests
// ============================================================================

TEST(MakeNumericAnyTest, CreatesNumericAny) {
    auto a = make_numeric_any(42);
    EXPECT_EQ(a.type_name(), "int");
    EXPECT_EQ(as<int>(a), 42);

    auto b = make_numeric_any(3.14);
    EXPECT_EQ(b.type_name(), "double");
}

// ============================================================================
// 16. Integer Edge Cases
// ============================================================================

TEST(IntegerEdgeTest, MaxInt) {
    int max_val = std::numeric_limits<int>::max();
    numeric_any a{max_val};
    EXPECT_EQ(as<int>(a), max_val);
}

TEST(IntegerEdgeTest, MinInt) {
    int min_val = std::numeric_limits<int>::min();
    numeric_any a{min_val};
    EXPECT_EQ(as<int>(a), min_val);
    EXPECT_FALSE(a.is_nonnegative());
}

TEST(IntegerEdgeTest, MaxUnsignedInt) {
    unsigned int max_val = std::numeric_limits<unsigned int>::max();
    numeric_any a{max_val};
    EXPECT_EQ(as<unsigned int>(a), max_val);
}

TEST(IntegerEdgeTest, MaxLongLong) {
    long long max_val = std::numeric_limits<long long>::max();
    numeric_any a{max_val};
    EXPECT_EQ(as<long long>(a), max_val);
}

TEST(IntegerEdgeTest, MinLongLong) {
    long long min_val = std::numeric_limits<long long>::min();
    numeric_any a{min_val};
    EXPECT_EQ(as<long long>(a), min_val);
    EXPECT_FALSE(a.is_nonnegative());
}

TEST(IntegerEdgeTest, MaxUnsignedLongLong) {
    unsigned long long max_val = std::numeric_limits<unsigned long long>::max();
    numeric_any a{max_val};
    EXPECT_EQ(as<unsigned long long>(a), max_val);
}

// ============================================================================
// 17. Stress / Chained Operations Tests
// ============================================================================

TEST(StressTest, ChainedOperations) {
    numeric_any a{10};
    a += 5; // 15
    a -= 3; // 12
    a *= 2; // 24
    a /= 4; // 6
    EXPECT_EQ(as<int>(a), 6);
}

TEST(StressTest, MixedTypeChainedOperations) {
    numeric_any a{10}; // int
    a += 3.5;          // double 13.5
    a -= 1.5f;         // double 12.0
    a *= 2;            // double 24.0
    a /= 3;            // double 8.0
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_DOUBLE_EQ(as<double>(a), 8.0);
}

TEST(StressTest, VectorOfNumericAny) {
    std::vector<numeric_any> vec;
    vec.emplace_back(42);
    vec.emplace_back(3.14);
    vec.emplace_back(true);
    vec.emplace_back(100ULL);
    vec.emplace_back(-1.5f);

    EXPECT_EQ(vec.size(), 5u);
    EXPECT_EQ(as<int>(vec[0]), 42);
    EXPECT_DOUBLE_EQ(as<double>(vec[1]), 3.14);
    EXPECT_EQ(as<bool>(vec[2]), true);
    EXPECT_EQ(as<unsigned long long>(vec[3]), 100ULL);
    EXPECT_FLOAT_EQ(as<float>(vec[4]), -1.5f);
}

// ============================================================================
// 18. Copy and Move Semantics
// ============================================================================

TEST(CopyMoveTest, CopyConstructor) {
    numeric_any a{42};
    numeric_any b{a};
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), 42);
    EXPECT_EQ(a <=> b, std::partial_ordering::equivalent);
}

TEST(CopyMoveTest, CopyAssignment) {
    numeric_any a{42};
    numeric_any b{3.14};
    b = a;
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), 42);
}

TEST(CopyMoveTest, MoveConstructor) {
    numeric_any a{42};
    numeric_any b{std::move(a)};
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), 42);
}

TEST(CopyMoveTest, MoveAssignment) {
    numeric_any a{42};
    numeric_any b{3.14};
    b = std::move(a);
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), 42);
}

// ============================================================================
// 19. Casting Policy Edge Cases (equal / strict / normal)
// ============================================================================

TEST(CastingPolicyEdgeTest, StrictBoolToIntAllowed) {
    numeric_any a{true};
    EXPECT_TRUE(from<int>(a).has_value());
    EXPECT_TRUE(from<bool>(a).has_value());
}

TEST(CastingPolicyEdgeTest, StrictBoolToDoubleAllowed) {
    numeric_any a{false};
    EXPECT_TRUE(from<double>(a).has_value());
}

TEST(CastingPolicyEdgeTest, StrictUnsignedToWiderUnsignedAllowed) {
    numeric_any a{255U};
    auto res = from<unsigned long long>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, 255ULL);
    // normal policy also allows it.
    auto res2 = from<unsigned long long, casting_policy::normal>(a);
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(*res2, 255ULL);
}

TEST(CastingPolicyEdgeTest, NormalPolicyFloatToIntTruncates) {
    numeric_any a{3.14f};
    auto res = from<int, casting_policy::normal>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, 3);
}

TEST(CastingPolicyEdgeTest, NormalPolicyIntToFloat) {
    numeric_any a{42};
    auto res = from<float, casting_policy::normal>(a);
    ASSERT_TRUE(res.has_value());
    EXPECT_FLOAT_EQ(*res, 42.0f);
}

TEST(CastingPolicyEdgeTest, EqualPolicyRejectsCrossType) {
    numeric_any a{42};
    EXPECT_FALSE((from<long long, casting_policy::equal>(a).has_value()));
    EXPECT_FALSE((from<double, casting_policy::equal>(a).has_value()));
}

// ============================================================================
// 20. Subnormal / Denormal Comprehensive Tests
// ============================================================================

TEST(SubnormalTest, FloatSubnormalOperation) {
    float sub = std::numeric_limits<float>::denorm_min();
    numeric_any a{sub};
    a += std::numeric_limits<float>::denorm_min();
    EXPECT_GT(as<float>(a), 0.0f);
}

TEST(SubnormalTest, DoubleSubnormalMultiply) {
    double sub = std::numeric_limits<double>::denorm_min();
    numeric_any a{sub};
    a *= 2.0;
    EXPECT_GT(as<double>(a), 0.0);
}

// ============================================================================
// 21. Comprehensive All Types Round-Trip Test
// ============================================================================

TEST(RoundTripTest, BoolRoundTrip) {
    for (bool v : {true, false}) {
        numeric_any a{v};
        EXPECT_EQ(as<bool>(a), v);
    }
}

TEST(RoundTripTest, SignedCharRoundTrip) {
    for (auto v : {static_cast<signed char>(-128), static_cast<signed char>(-1), static_cast<signed char>(0),
                   static_cast<signed char>(1), static_cast<signed char>(127)}) {
        numeric_any a{v};
        EXPECT_EQ(as<signed char>(a), v);
    }
}

TEST(RoundTripTest, UnsignedCharRoundTrip) {
    for (auto v : {static_cast<unsigned char>(0), static_cast<unsigned char>(1), static_cast<unsigned char>(128),
                   static_cast<unsigned char>(255)}) {
        numeric_any a{v};
        EXPECT_EQ(as<unsigned char>(a), v);
    }
}

TEST(RoundTripTest, ShortRoundTrip) {
    for (short v : {short{-32768}, short{-1}, short{0}, short{1}, short{32767}}) {
        numeric_any a{v};
        EXPECT_EQ(as<short>(a), v);
    }
}

TEST(RoundTripTest, UnsignedShortRoundTrip) {
    for (auto v :
         {static_cast<unsigned short>(0), static_cast<unsigned short>(1), static_cast<unsigned short>(65535)}) {
        numeric_any a{v};
        EXPECT_EQ(as<unsigned short>(a), v);
    }
}

TEST(RoundTripTest, IntRoundTrip) {
    for (int v : {std::numeric_limits<int>::min(), -1, 0, 1, std::numeric_limits<int>::max()}) {
        numeric_any a{v};
        EXPECT_EQ(as<int>(a), v);
    }
}

TEST(RoundTripTest, UnsignedIntRoundTrip) {
    for (unsigned int v : {0U, 1U, std::numeric_limits<unsigned int>::max()}) {
        numeric_any a{v};
        EXPECT_EQ(as<unsigned int>(a), v);
    }
}

TEST(RoundTripTest, LongRoundTrip) {
    for (long v : {std::numeric_limits<long>::min(), -1L, 0L, 1L, std::numeric_limits<long>::max()}) {
        numeric_any a{v};
        EXPECT_EQ(as<long>(a), v);
    }
}

TEST(RoundTripTest, UnsignedLongRoundTrip) {
    for (unsigned long v : {0UL, 1UL, std::numeric_limits<unsigned long>::max()}) {
        numeric_any a{v};
        EXPECT_EQ(as<unsigned long>(a), v);
    }
}

TEST(RoundTripTest, LongLongRoundTrip) {
    for (long long v : {std::numeric_limits<long long>::min(), -1LL, 0LL, 1LL, std::numeric_limits<long long>::max()}) {
        numeric_any a{v};
        EXPECT_EQ(as<long long>(a), v);
    }
}

TEST(RoundTripTest, UnsignedLongLongRoundTrip) {
    for (unsigned long long v : {0ULL, 1ULL, std::numeric_limits<unsigned long long>::max()}) {
        numeric_any a{v};
        EXPECT_EQ(as<unsigned long long>(a), v);
    }
}

TEST(RoundTripTest, FloatRoundTrip) {
    for (float v : {0.0f, -0.0f, 1.0f, -1.0f, 3.14f, -3.14f, std::numeric_limits<float>::min(),
                    std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()}) {
        numeric_any a{v};
        EXPECT_FLOAT_EQ(as<float>(a), v);
    }
}

TEST(RoundTripTest, DoubleRoundTrip) {
    for (double v : {0.0, -0.0, 1.0, -1.0, 3.14159265358979, -3.14159265358979, std::numeric_limits<double>::min(),
                     std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest()}) {
        numeric_any a{v};
        EXPECT_DOUBLE_EQ(as<double>(a), v);
    }
}

TEST(RoundTripTest, LongDoubleRoundTrip) {
    for (long double v : {0.0L, 1.0L, -1.0L, 3.14159265358979L, std::numeric_limits<long double>::min(),
                          std::numeric_limits<long double>::max()}) {
        numeric_any a{v};
        EXPECT_DOUBLE_EQ(static_cast<double>(as<long double>(a)), static_cast<double>(v));
    }
}

// ============================================================================
// 22. Unary Operator Tests (new: operator- / operator+ / ++ / --)
// ============================================================================

TEST(UnaryOperatorTest, UnaryPlusReturnsCopy) {
    numeric_any a{42};
    auto b = +a;
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), 42);
    EXPECT_EQ(a <=> b, std::partial_ordering::equivalent);
    EXPECT_EQ(as<int>(a), 42);
}

TEST(UnaryOperatorTest, UnaryMinusInt) {
    numeric_any a{42};
    auto b = -a;
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), -42);

    numeric_any c{-42};
    auto d = -c;
    EXPECT_EQ(as<int>(d), 42);
    EXPECT_EQ(as<int>(a), 42);
}

TEST(UnaryOperatorTest, UnaryMinusDouble) {
    numeric_any a{3.14};
    auto b = -a;
    EXPECT_EQ(b.type_name(), "double");
    EXPECT_DOUBLE_EQ(as<double>(b), -3.14);
}

TEST(UnaryOperatorTest, UnaryMinusFloat) {
    numeric_any a{1.5f};
    auto b = -a;
    EXPECT_EQ(b.type_name(), "float");
    EXPECT_FLOAT_EQ(as<float>(b), -1.5f);
}

TEST(UnaryOperatorTest, UnaryMinusLongDouble) {
    numeric_any a{2.5L};
    auto b = -a;
    EXPECT_EQ(b.type_name(), "long double");
    EXPECT_DOUBLE_EQ(static_cast<double>(as<long double>(b)), -2.5);
}

TEST(UnaryOperatorTest, UnaryMinusUnsignedWraps) {
    numeric_any a{5U};
    auto b = -a;
    EXPECT_EQ(b.type_name(), "unsigned int");
    EXPECT_EQ(as<unsigned int>(b), 4294967291U);
}

TEST(UnaryOperatorTest, UnaryMinusBool) {
    numeric_any a{true};
    auto b = -a;
    EXPECT_EQ(b.type_name(), "int");
    EXPECT_EQ(as<int>(b), -1);
}

TEST(UnaryOperatorTest, PrefixIncrementInt) {
    numeric_any a{41};
    auto& ref = ++a;
    EXPECT_EQ(&ref, &a); // returns *this
    EXPECT_EQ(a.type_name(), "int");
    EXPECT_EQ(as<int>(a), 42);
}

TEST(UnaryOperatorTest, PostfixIncrementInt) {
    numeric_any a{41};
    auto old = a++;
    EXPECT_EQ(old.type_name(), "int");
    EXPECT_EQ(as<int>(old), 41);
    EXPECT_EQ(as<int>(a), 42);
}

TEST(UnaryOperatorTest, PrefixDecrementInt) {
    numeric_any a{43};
    auto& ref = --a;
    EXPECT_EQ(&ref, &a);
    EXPECT_EQ(a.type_name(), "int");
    EXPECT_EQ(as<int>(a), 42);
}

TEST(UnaryOperatorTest, PostfixDecrementInt) {
    numeric_any a{43};
    auto old = a--;
    EXPECT_EQ(as<int>(old), 43);
    EXPECT_EQ(as<int>(a), 42);
}

TEST(UnaryOperatorTest, IncrementPreservesType) {
    numeric_any a{1.5};
    ++a;
    EXPECT_EQ(a.type_name(), "double");
    EXPECT_DOUBLE_EQ(as<double>(a), 2.5);

    numeric_any b{100ULL};
    ++b;
    EXPECT_EQ(b.type_name(), "unsigned long long");
    EXPECT_EQ(as<unsigned long long>(b), 101ULL);
}

TEST(UnaryOperatorTest, DecrementPreservesType) {
    numeric_any a{3.5f};
    --a;
    EXPECT_EQ(a.type_name(), "float");
    EXPECT_FLOAT_EQ(as<float>(a), 2.5f);

    numeric_any b{10L};
    --b;
    EXPECT_EQ(b.type_name(), "long");
    EXPECT_EQ(as<long>(b), 9L);
}

TEST(UnaryOperatorTest, IncrementBool) {
    numeric_any a{false};
    ++a;
    EXPECT_EQ(as<bool>(a), true);

    numeric_any b{true};
    ++b;
    EXPECT_EQ(as<bool>(b), true);
}

TEST(UnaryOperatorTest, DecrementBool) {
    numeric_any a{true};
    --a;
    EXPECT_EQ(as<bool>(a), false);

    numeric_any b{false};
    --b;
    EXPECT_EQ(as<bool>(b), true);
}

// ============================================================================
// 23. std::formatter Format-String Parsing Tests (char)
//     (new: numeric_any_parser supports align/sign/#/0/width/precision/L/type)
// ============================================================================

TEST(FormatterTest, FormatBasic) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{}", a), "42");

    numeric_any b{3.14};
    EXPECT_EQ(std::format("{}", b), "3.14");

    numeric_any c{true};
    EXPECT_EQ(std::format("{}", c), "true");
}

TEST(FormatterTest, FormatIntegerSpecifiers) {
    numeric_any a{255};
    EXPECT_EQ(std::format("{:d}", a), "255");
    EXPECT_EQ(std::format("{:x}", a), "ff");
    EXPECT_EQ(std::format("{:X}", a), "FF");
    EXPECT_EQ(std::format("{:o}", a), "377");
    EXPECT_EQ(std::format("{:b}", a), "11111111");
    EXPECT_EQ(std::format("{:B}", a), "11111111");
}

TEST(FormatterTest, FormatFloatSpecifiers) {
    numeric_any a{3.14};
    EXPECT_EQ(std::format("{:f}", a), "3.140000");
    EXPECT_EQ(std::format("{:e}", a), "3.140000e+00");
    EXPECT_EQ(std::format("{:g}", a), "3.14");
    // Note: this STL emits hex-float without the "0x" prefix
    EXPECT_EQ(std::format("{:a}", a), "1.91eb851eb851fp+1");

    numeric_any b{2.0};
    EXPECT_EQ(std::format("{:a}", b), "1p+1");
}

TEST(FormatterTest, FormatWidth) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{:6d}", a), "    42");
    EXPECT_EQ(std::format("{:2d}", a), "42"); // width smaller than needed
}

TEST(FormatterTest, FormatAlignment) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{:<6d}", a), "42    ");
    EXPECT_EQ(std::format("{:>6d}", a), "    42");
    EXPECT_EQ(std::format("{:^6d}", a), "  42  ");
}

TEST(FormatterTest, FormatFillAndAlign) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{:*<6d}", a), "42****");
    EXPECT_EQ(std::format("{:*>6d}", a), "****42");
    EXPECT_EQ(std::format("{:*^6d}", a), "**42**");
    EXPECT_EQ(std::format("{:0>6d}", a), "000042");
}

TEST(FormatterTest, FormatZeroPadding) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{:06d}", a), "000042");
    EXPECT_EQ(std::format("{:04d}", a), "0042");
}

TEST(FormatterTest, FormatPrecision) {
    numeric_any a{3.14159};
    EXPECT_EQ(std::format("{:.2f}", a), "3.14");
    EXPECT_EQ(std::format("{:.4f}", a), "3.1416");
    EXPECT_EQ(std::format("{:.1e}", a), "3.1e+00");
    EXPECT_EQ(std::format("{:.2g}", a), "3.1");
}

TEST(FormatterTest, FormatSign) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{:+d}", a), "+42");
    EXPECT_EQ(std::format("{: d}", a), " 42");
    EXPECT_EQ(std::format("{:-d}", a), "42");

    numeric_any b{-42};
    EXPECT_EQ(std::format("{:+d}", b), "-42");
    EXPECT_EQ(std::format("{: d}", b), "-42");
    EXPECT_EQ(std::format("{:-d}", b), "-42");
}

TEST(FormatterTest, FormatAlternateForm) {
    numeric_any a{255};
    EXPECT_EQ(std::format("{:#x}", a), "0xff");
    EXPECT_EQ(std::format("{:#X}", a), "0XFF");
    EXPECT_EQ(std::format("{:#o}", a), "0377");
    EXPECT_EQ(std::format("{:#b}", a), "0b11111111");
}

TEST(FormatterTest, FormatDynamicWidth) {
    numeric_any a{42};
    EXPECT_EQ(std::format("{:{}d}", a, 6), "    42");
    EXPECT_EQ(std::format("{:*<{}d}", a, 6), "42****");
}

TEST(FormatterTest, FormatDynamicPrecision) {
    numeric_any a{3.14159};
    EXPECT_EQ(std::format("{:.{}f}", a, 1), "3.1");
    EXPECT_EQ(std::format("{:.{}f}", a, 3), "3.142");
}

TEST(FormatterTest, EraseUnusedPattern) {
    numeric_any a{42.324};
    EXPECT_EQ(std::format("{:*>0.0f}", a), std::format("{:f}", 42.324));
}

TEST(FormatterTest, WrongTypeFormat) {
    numeric_any a{42.324}, b{12};
    try {
        EXPECT_THROW((void)std::vformat("{:d}", std::make_format_args(a)), std::format_error);
    } catch (std::format_error& e) {
        EXPECT_STREQ(e.what(), "Cannot use integer format specifier with a float.");
    }
    try {
        EXPECT_THROW((void)std::vformat("{:.2f}", std::make_format_args(b)), std::format_error);
    } catch (std::format_error& e) {
        EXPECT_STREQ(e.what(), "Cannot use float format specifier with an integer.");
    }
}

TEST(FormatterTest, FormatErrorInvalidSpecifier) {
    numeric_any a{42};
    EXPECT_THROW((void)std::vformat("{:q}", std::make_format_args(a)), std::format_error);
    EXPECT_THROW((void)std::vformat("{:qd}", std::make_format_args(a)), std::format_error);
}

// ============================================================================
// 24. std::formatter Wide-Character (wchar_t) Tests
// ============================================================================

TEST(WideFormatterTest, FormatBasic) {
    numeric_any a{42};
    EXPECT_EQ(std::format(L"{}", a), L"42");

    numeric_any b{3.14};
    EXPECT_EQ(std::format(L"{}", b), L"3.14");

    numeric_any c{true};
    EXPECT_EQ(std::format(L"{}", c), L"true");
}

TEST(WideFormatterTest, FormatSpecifiers) {
    numeric_any a{255};
    EXPECT_EQ(std::format(L"{:x}", a), L"ff");
    EXPECT_EQ(std::format(L"{:o}", a), L"377");
    EXPECT_EQ(std::format(L"{:#x}", a), L"0xff");

    numeric_any b{3.14159};
    EXPECT_EQ(std::format(L"{:.2f}", b), L"3.14");
}

TEST(WideFormatterTest, FormatWidthAndAlign) {
    numeric_any a{42};
    EXPECT_EQ(std::format(L"{:06d}", a), L"000042");
    EXPECT_EQ(std::format(L"{:<6d}", a), L"42    ");
    EXPECT_EQ(std::format(L"{:*>6d}", a), L"****42");
}

TEST(WideFormatterTest, EraseUnusedPattern) {
    numeric_any a{42.324};
    EXPECT_EQ(std::format(L"{:*>0.0f}", a), std::format(L"{:f}", 42.324));
}

TEST(WideFormatterTest, WrongTypeFormat) {
    numeric_any a{42.324}, b{12};
    try {
        EXPECT_THROW((void)std::vformat(L"{:d}", std::make_wformat_args(a)), std::format_error);
    } catch (std::format_error& e) {
        EXPECT_STREQ(e.what(), "Cannot use integer format specifier with a float.");
    }
    try {
        EXPECT_THROW((void)std::vformat(L"{:.2f}", std::make_wformat_args(b)), std::format_error);
    } catch (std::format_error& e) {
        EXPECT_STREQ(e.what(), "Cannot use float format specifier with an integer.");
    }
}

TEST(WideFormatterTest, FormatErrorInvalidSpecifier) {
    numeric_any a{42};
    EXPECT_THROW((void)std::vformat(L"{:q}", std::make_wformat_args(a)), std::format_error);
    EXPECT_THROW((void)std::vformat(L"{:qd}", std::make_wformat_args(a)), std::format_error);
}

// ============================================================================
// 25. Additional wostream Output Tests (float / unsigned / long double)
// ============================================================================

TEST(WideStreamOutputTest, OutputFloat) {
    numeric_any a{3.14f};
    std::ostringstream oss;
    std::wostringstream woss;
    oss << a;
    woss << a;
    EXPECT_NE(oss.str().find("3.14"), std::string::npos);
    EXPECT_NE(woss.str().find(L"3.14"), std::wstring::npos);
}

TEST(WideStreamOutputTest, OutputUnsignedInt) {
    numeric_any a{123U};
    std::ostringstream oss;
    std::wostringstream woss;
    oss << a;
    woss << a;
    EXPECT_EQ(oss.str(), "123");
    EXPECT_EQ(woss.str(), L"123");
}

TEST(WideStreamOutputTest, OutputUnsignedLongLong) {
    numeric_any a{18446744073709551615ULL};
    std::ostringstream oss;
    std::wostringstream woss;
    oss << a;
    woss << a;
    EXPECT_EQ(oss.str(), "18446744073709551615");
    EXPECT_EQ(woss.str(), L"18446744073709551615");
}

TEST(WideStreamOutputTest, OutputLongDouble) {
    numeric_any a{2.5L};
    std::ostringstream oss;
    std::wostringstream woss;
    oss << a;
    woss << a;
    EXPECT_NE(oss.str().find("2.5"), std::string::npos);
    EXPECT_NE(woss.str().find(L"2.5"), std::wstring::npos);
}

TEST(WideStreamOutputTest, OutputShort) {
    numeric_any a{short{-5}};
    std::ostringstream oss;
    std::wostringstream woss;
    oss << a;
    woss << a;
    EXPECT_EQ(oss.str(), "-5");
    EXPECT_EQ(woss.str(), L"-5");
}

// ============================================================================
// 26. visit() Function Tests (new in v2.0.0)
// ============================================================================

TEST(VisitTest, VisitReturnsTypedResult) {
    numeric_any a{42};
    auto doubled = visit([](auto v) -> long double { return static_cast<long double>(v) * 2; }, a);
    static_assert(std::is_same_v<decltype(doubled), long double>);
    EXPECT_DOUBLE_EQ(doubled, 84.0L);
}

TEST(VisitTest, VisitDouble) {
    numeric_any a{3.14};
    auto r = visit([](auto v) -> double { return static_cast<double>(v) + 1.0; }, a);
    EXPECT_DOUBLE_EQ(r, 4.14);
}

TEST(VisitTest, VisitBool) {
    numeric_any a{true};
    EXPECT_TRUE(visit([](auto v) -> bool { return static_cast<bool>(v); }, a));
    numeric_any b{false};
    EXPECT_FALSE(visit([](auto v) -> bool { return static_cast<bool>(v); }, b));
}

TEST(VisitTest, VisitVoidLambdaSideEffect) {
    numeric_any a{10};
    int captured = 0;
    visit([&captured](auto v) { captured = v; }, a);
    EXPECT_EQ(captured, 10);
}

TEST(VisitTest, VisitConstNumericAny) {
    const numeric_any a{7};
    auto r = visit([](auto v) -> long long { return static_cast<long long>(v) + 1; }, a);
    EXPECT_EQ(r, 8);
}

TEST(VisitTest, VisitExposesExactInnerType) {
    auto sc = static_cast<signed char>(5);
    numeric_any a{sc};
    EXPECT_TRUE(visit([](auto v) { return std::is_same_v<decltype(v), signed char>; }, a));

    numeric_any b{5U};
    EXPECT_TRUE(visit([](auto v) { return std::is_same_v<decltype(v), unsigned int>; }, b));

    numeric_any c{5.5f};
    EXPECT_TRUE(visit([](auto v) { return std::is_same_v<decltype(v), float>; }, c));

    numeric_any d{5.5L};
    EXPECT_TRUE(visit([](auto v) { return std::is_same_v<decltype(v), long double>; }, d));
}

// ============================================================================
// 27. std::common_type Specializations (new in v2.0.0)
// ============================================================================

static_assert(std::is_same_v<std::common_type_t<int, numeric_any>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<numeric_any, int>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<double, numeric_any>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<numeric_any, double>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<long double, numeric_any>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<numeric_any, long double>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<unsigned int, numeric_any>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<numeric_any, bool>, numeric_any>);
static_assert(std::is_same_v<std::common_type_t<numeric_any, numeric_any>, numeric_any>);

TEST(CommonTypeTest, CommonTypeResolvesToNumericAny) {
    EXPECT_TRUE((std::is_same_v<std::common_type_t<int, numeric_any>, numeric_any>));
    EXPECT_TRUE((std::is_same_v<std::common_type_t<numeric_any, double>, numeric_any>));
}

// ============================================================================
// 28. sign_unambiguous_arithmetic Concept (new in v2.0.0)
// ============================================================================

static_assert(sign_unambiguous_arithmetic<bool>);
static_assert(sign_unambiguous_arithmetic<signed char>);
static_assert(sign_unambiguous_arithmetic<unsigned char>);
static_assert(sign_unambiguous_arithmetic<short>);
static_assert(sign_unambiguous_arithmetic<unsigned short>);
static_assert(sign_unambiguous_arithmetic<int>);
static_assert(sign_unambiguous_arithmetic<unsigned int>);
static_assert(sign_unambiguous_arithmetic<long>);
static_assert(sign_unambiguous_arithmetic<unsigned long>);
static_assert(sign_unambiguous_arithmetic<long long>);
static_assert(sign_unambiguous_arithmetic<unsigned long long>);
static_assert(sign_unambiguous_arithmetic<float>);
static_assert(sign_unambiguous_arithmetic<double>);
static_assert(sign_unambiguous_arithmetic<long double>);
static_assert(!sign_unambiguous_arithmetic<char>);
static_assert(!sign_unambiguous_arithmetic<wchar_t>);

static_assert(!sign_unambiguous_arithmetic<char8_t>);
static_assert(!sign_unambiguous_arithmetic<char16_t>);
static_assert(!sign_unambiguous_arithmetic<char32_t>);
static_assert(!sign_unambiguous_arithmetic<std::string>);

TEST(ConceptTest, ConceptAcceptsOnlySupportedArithmeticTypes) {
    EXPECT_TRUE(sign_unambiguous_arithmetic<int>);
    EXPECT_TRUE(sign_unambiguous_arithmetic<long double>);
    EXPECT_FALSE(sign_unambiguous_arithmetic<char>);
    EXPECT_FALSE(sign_unambiguous_arithmetic<wchar_t>);
    EXPECT_FALSE(sign_unambiguous_arithmetic<char8_t>);
    EXPECT_FALSE(sign_unambiguous_arithmetic<char16_t>);
    EXPECT_FALSE(sign_unambiguous_arithmetic<char32_t>);
    EXPECT_FALSE(sign_unambiguous_arithmetic<std::string>);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
