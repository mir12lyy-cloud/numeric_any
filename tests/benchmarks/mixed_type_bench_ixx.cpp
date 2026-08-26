// Benchmark: Mixed-type operations �?cross-type arithmetic & comparison
// Compares numeric_any vs native. Extended: int, long long, ull, float, double, long double
#include <benchmark/benchmark.h>
#include <random>
#include <variant>
#include <vector>

import cy.maths.numeric_any;

using namespace cy::maths;
static constexpr size_t N = 1000000;

using llong   = long long;
using ullong  = unsigned long long;
using ldouble = long double;

static std::vector<int> mk_int(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> d(-10000, 10000);
    std::vector<int> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<llong> mk_ll(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<llong> d(-10000000000LL, 10000000000LL);
    std::vector<llong> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<ullong> mk_ull(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<ullong> d(0, 18000000000000000000ULL);
    std::vector<ullong> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<float> mk_flt(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> d(-100.f, 100.f);
    std::vector<float> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<double> mk_dbl(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> d(-100., 100.);
    std::vector<double> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<ldouble> mk_ldbl(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<ldouble> d(-100.L, 100.L);
    std::vector<ldouble> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<unsigned int> mk_uint(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<unsigned int> d(0, 20000);
    std::vector<unsigned int> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}

#define BENCH_MIXED_ADD(T1, n1, T2, n2, V1)                                                                            \
    static void BM_Native_Add_##n1##_##n2(benchmark::State& s) {                                                       \
        auto d1 = mk_##n1(N);                                                                                          \
        auto d2 = mk_##n2(N);                                                                                          \
        for (auto _ : s) {                                                                                             \
            std::common_type_t<T1, T2> acc = 0;                                                                        \
            for (size_t i = 0; i < N; ++i) {                                                                           \
                acc += d1[i];                                                                                          \
                acc += d2[i];                                                                                          \
            }                                                                                                          \
            benchmark::DoNotOptimize(acc);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N * 2);                                                                   \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Add_##n1##_##n2);                                                                              \
    static void BM_NumericAny_Add_##n1##_##n2(benchmark::State& s) {                                                   \
        auto d1 = mk_##n1(N);                                                                                          \
        auto d2 = mk_##n2(N);                                                                                          \
        for (auto _ : s) {                                                                                             \
            numeric_any acc{V1};                                                                                       \
            for (size_t i = 0; i < N; ++i) {                                                                           \
                acc += d1[i];                                                                                          \
                acc += d2[i];                                                                                          \
            }                                                                                                          \
            benchmark::DoNotOptimize(acc);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N * 2);                                                                   \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Add_##n1##_##n2);
BENCH_MIXED_ADD(int, int, double, dbl, 0)
BENCH_MIXED_ADD(int, int, unsigned int, uint, 0)
BENCH_MIXED_ADD(int, int, ldouble, ldbl, 0)
BENCH_MIXED_ADD(float, flt, double, dbl, 0.0f)
BENCH_MIXED_ADD(float, flt, ldouble, ldbl, 0.0f)
BENCH_MIXED_ADD(llong, ll, double, dbl, 0LL)
BENCH_MIXED_ADD(llong, ll, ullong, ull, 0LL)
BENCH_MIXED_ADD(double, dbl, ldouble, ldbl, 0.0)
#undef BENCH_MIXED_ADD

#define BENCH_MIXED_CMP(T1, n1, T2, n2)                                                                                \
    static void BM_Native_Compare_##n1##_##n2(benchmark::State& s) {                                                   \
        auto d1 = mk_##n1(N);                                                                                          \
        auto d2 = mk_##n2(N);                                                                                          \
        for (auto _ : s) {                                                                                             \
            int lt = 0, gt = 0;                                                                                        \
            for (size_t i = 0; i < N; ++i) {                                                                           \
                auto r =                                                                                               \
                    static_cast<std::common_type_t<T1, T2>>(d1[i]) <=> static_cast<std::common_type_t<T1, T2>>(d2[i]); \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r > 0)                                                                                        \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + gt);                                                                         \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Compare_##n1##_##n2);                                                                          \
    static void BM_NumericAny_Compare_##n1##_##n2(benchmark::State& s) {                                               \
        auto d1 = mk_##n1(N);                                                                                          \
        auto d2 = mk_##n2(N);                                                                                          \
        std::vector<numeric_any> na;                                                                                   \
        na.reserve(N);                                                                                                 \
        for (auto v : d1)                                                                                              \
            na.emplace_back(v);                                                                                        \
        for (auto _ : s) {                                                                                             \
            int lt = 0, gt = 0;                                                                                        \
            for (size_t i = 0; i < N; ++i) {                                                                           \
                auto r = na[i] <=> d2[i];                                                                              \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r > 0)                                                                                        \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + gt);                                                                         \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Compare_##n1##_##n2);
BENCH_MIXED_CMP(int, int, double, dbl)
BENCH_MIXED_CMP(int, int, ldouble, ldbl)
BENCH_MIXED_CMP(int, int, unsigned int, uint)
BENCH_MIXED_CMP(double, dbl, int, int)
BENCH_MIXED_CMP(double, dbl, ldouble, ldbl)
BENCH_MIXED_CMP(llong, ll, double, dbl)
BENCH_MIXED_CMP(ldouble, ldbl, int, int)
#undef BENCH_MIXED_CMP

#define BENCH_BOTH_ERASED_CMP(T1, n1, T2, n2)                                                                          \
    static void BM_NumericAny_Both_Compare_##n1##_##n2(benchmark::State& s) {                                          \
        auto d1 = mk_##n1(N);                                                                                          \
        auto d2 = mk_##n2(N);                                                                                          \
        std::vector<numeric_any> na1, na2;                                                                             \
        na1.reserve(N);                                                                                                \
        na2.reserve(N);                                                                                                \
        for (size_t i = 0; i < N; ++i) {                                                                               \
            na1.emplace_back(d1[i]);                                                                                   \
            na2.emplace_back(d2[i]);                                                                                   \
        }                                                                                                              \
        for (auto _ : s) {                                                                                             \
            int lt = 0, gt = 0;                                                                                        \
            for (size_t i = 0; i < N; ++i) {                                                                           \
                auto r = na1[i] <=> na2[i];                                                                            \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r > 0)                                                                                        \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + gt);                                                                         \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Both_Compare_##n1##_##n2);                                                                 \
    static void BM_StdVariant_Both_Compare_##n1##_##n2(benchmark::State& s) {                                          \
        auto d1   = mk_##n1(N);                                                                                        \
        auto d2   = mk_##n2(N);                                                                                        \
        using Var = std::variant<int, llong, double, ldouble, ullong, float>;                                          \
        std::vector<Var> v1, v2;                                                                                       \
        v1.reserve(N);                                                                                                 \
        v2.reserve(N);                                                                                                 \
        for (size_t i = 0; i < N; ++i) {                                                                               \
            v1.emplace_back(d1[i]);                                                                                    \
            v2.emplace_back(d2[i]);                                                                                    \
        }                                                                                                              \
        for (auto _ : s) {                                                                                             \
            int lt = 0, gt = 0;                                                                                        \
            for (size_t i = 0; i < N; ++i) {                                                                           \
                auto r = std::visit(                                                                                   \
                    [](auto a, auto b) -> std::partial_ordering {                                                      \
                        using CT = std::common_type_t<decltype(a), decltype(b)>;                                       \
                        return static_cast<CT>(a) <=> static_cast<CT>(b);                                              \
                    },                                                                                                 \
                    v1[i], v2[i]);                                                                                     \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r > 0)                                                                                        \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + gt);                                                                         \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdVariant_Both_Compare_##n1##_##n2);
BENCH_BOTH_ERASED_CMP(int, int, double, dbl)
BENCH_BOTH_ERASED_CMP(int, int, ldouble, ldbl)
BENCH_BOTH_ERASED_CMP(double, dbl, ldouble, ldbl)
#undef BENCH_BOTH_ERASED_CMP

static void BM_Native_Mul_Int_Double(benchmark::State& s) {
    auto ints = mk_int(N);
    auto dbls = mk_dbl(N);
    for (auto _ : s) {
        double acc = 1.0;
        for (size_t i = 0; i < N; ++i)
            acc *= ints[i] * dbls[i];
        benchmark::DoNotOptimize(acc);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Native_Mul_Int_Double);
static void BM_NumericAny_Mul_Int_Double(benchmark::State& s) {
    auto ints = mk_int(N);
    auto dbls = mk_dbl(N);
    for (auto _ : s) {
        numeric_any acc{1.0};
        for (size_t i = 0; i < N; ++i) {
            acc *= ints[i];
            acc *= dbls[i];
        }
        benchmark::DoNotOptimize(acc);
    }
    s.SetItemsProcessed(s.iterations() * N * 2);
}
BENCHMARK(BM_NumericAny_Mul_Int_Double);

BENCHMARK_MAIN();
