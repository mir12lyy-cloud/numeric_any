// Benchmark: Type conversion & policy overhead expanded types
#include <benchmark/benchmark.h>
#define DISABLE_FORMAT_IN_NUMERIC_ANY
#include "../../src/numeric_any.hpp"
#include <any>
#include <random>
#include <variant>
#include <vector>

using namespace casyyy::maths;
static constexpr size_t N = 1000000;

using llong   = long long;
using ullong  = unsigned long long;
using ldouble = long double;

static std::vector<int> mk_int(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> d(-100000, 100000);
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
    std::uniform_real_distribution<float> d(-1000.f, 1000.f);
    std::vector<float> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<double> mk_dbl(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> d(-1000., 1000.);
    std::vector<double> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<ldouble> mk_ldbl(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<ldouble> d(-1000.L, 1000.L);
    std::vector<ldouble> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}

// ── Unchecked vs Strict same-type cast ──
#define BENCH_CAST_SAME(T, tname)                                                                                      \
    static void BM_Unchecked_Same_##tname(benchmark::State& s) {                                                       \
        auto data = mk_##tname(N);                                                                                     \
        std::vector<numeric_any> na;                                                                                   \
        na.reserve(N);                                                                                                 \
        for (auto v : data)                                                                                            \
            na.emplace_back(v);                                                                                        \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto& a : na)                                                                                         \
                sum += unchecked_numeric_cast<T>(a);                                                                   \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Unchecked_Same_##tname);                                                                              \
    static void BM_Strict_Same_##tname(benchmark::State& s) {                                                          \
        auto data = mk_##tname(N);                                                                                     \
        std::vector<numeric_any> na;                                                                                   \
        na.reserve(N);                                                                                                 \
        for (auto v : data)                                                                                            \
            na.emplace_back(v);                                                                                        \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto& a : na) {                                                                                       \
                auto r = numeric_cast<T>(a);                                                                           \
                if (r) sum += *r;                                                                                      \
            }                                                                                                          \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Strict_Same_##tname);                                                                                 \
    static void BM_StdAny_Same_##tname(benchmark::State& s) {                                                          \
        auto data = mk_##tname(N);                                                                                     \
        std::vector<std::any> anys;                                                                                    \
        anys.reserve(N);                                                                                               \
        for (auto v : data)                                                                                            \
            anys.emplace_back(v);                                                                                      \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto& a : anys)                                                                                       \
                sum += std::any_cast<T>(a);                                                                            \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdAny_Same_##tname);
BENCH_CAST_SAME(int, int)
BENCH_CAST_SAME(llong, ll)
BENCH_CAST_SAME(float, flt)
BENCH_CAST_SAME(double, dbl)
BENCH_CAST_SAME(ldouble, ldbl)
#undef BENCH_CAST_SAME

// ── Variant get ──
#define BENCH_VARGET(T, tname)                                                                                         \
    static void BM_StdVariant_Same_##tname(benchmark::State& s) {                                                      \
        auto data = mk_##tname(N);                                                                                     \
        using Var = std::variant<int, llong, double, ldouble, float, ullong>;                                          \
        std::vector<Var> vars;                                                                                         \
        vars.reserve(N);                                                                                               \
        for (auto v : data)                                                                                            \
            vars.emplace_back(v);                                                                                      \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto& v : vars)                                                                                       \
                sum += std::get<T>(v);                                                                                 \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdVariant_Same_##tname);
BENCH_VARGET(int, int)
BENCH_VARGET(llong, ll)
BENCH_VARGET(double, dbl)
BENCH_VARGET(ldouble, ldbl)
#undef BENCH_VARGET

// ── Promotion cast: int �?long long ──
static void BM_Unchecked_Promote_IntToLL(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na)
            sum += unchecked_numeric_cast<llong>(a);
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Unchecked_Promote_IntToLL);
static void BM_Strict_Promote_IntToLL(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na) {
            auto r = numeric_cast<llong>(a);
            if (r) sum += *r;
        }
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Strict_Promote_IntToLL);

// ── Narrowing: int �?short ──
static void BM_Strict_Narrow_IntToShort(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        int fail = 0;
        for (auto& a : na) {
            auto r = numeric_cast<short>(a);
            if (!r) ++fail;
        }
        benchmark::DoNotOptimize(fail);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Strict_Narrow_IntToShort);
static void BM_Relaxed_Narrow_IntToShort(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        int ok = 0;
        for (auto& a : na) {
            auto r = numeric_cast<short, casting_policy::relaxed>(a);
            if (r) ++ok;
        }
        benchmark::DoNotOptimize(ok);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Relaxed_Narrow_IntToShort);

// ── Three policies: int �?long ──
static void BM_Strict_Policy_IntToLong(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na) {
            auto r = numeric_cast<long, casting_policy::strict>(a);
            if (r) sum += *r;
        }
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Strict_Policy_IntToLong);
static void BM_Normal_Policy_IntToLong(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na) {
            auto r = numeric_cast<long, casting_policy::normal>(a);
            if (r) sum += *r;
        }
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Normal_Policy_IntToLong);
static void BM_Relaxed_Policy_IntToLong(benchmark::State& s) {
    auto data = mk_int(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na) {
            auto r = numeric_cast<long, casting_policy::relaxed>(a);
            if (r) sum += *r;
        }
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Relaxed_Policy_IntToLong);

// ── Cross-type: double �?int, long double �?int (relaxed) ──
static void BM_Relaxed_Cross_DoubleToInt(benchmark::State& s) {
    auto data = mk_dbl(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na) {
            auto r = numeric_cast<int, casting_policy::relaxed>(a);
            if (r) sum += *r;
        }
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Relaxed_Cross_DoubleToInt);
static void BM_Relaxed_Cross_LDoubleToInt(benchmark::State& s) {
    auto data = mk_ldbl(N);
    std::vector<numeric_any> na;
    na.reserve(N);
    for (auto v : data)
        na.emplace_back(v);
    for (auto _ : s) {
        ldouble sum = 0;
        for (auto& a : na) {
            auto r = numeric_cast<int, casting_policy::relaxed>(a);
            if (r) sum += *r;
        }
        benchmark::DoNotOptimize(sum);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Relaxed_Cross_LDoubleToInt);

// ── Round-trip ──
#define BENCH_RT(T, tname)                                                                                             \
    static void BM_NumericAny_RoundTrip_##tname(benchmark::State& s) {                                                 \
        auto data = mk_##tname(N);                                                                                     \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto v : data) {                                                                                      \
                numeric_any a{v};                                                                                      \
                sum += unchecked_numeric_cast<T>(a);                                                                   \
            }                                                                                                          \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_RoundTrip_##tname);                                                                        \
    static void BM_StdAny_RoundTrip_##tname(benchmark::State& s) {                                                     \
        auto data = mk_##tname(N);                                                                                     \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto v : data) {                                                                                      \
                std::any a{v};                                                                                         \
                sum += std::any_cast<T>(a);                                                                            \
            }                                                                                                          \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdAny_RoundTrip_##tname);                                                                            \
    static void BM_StdVariant_RoundTrip_##tname(benchmark::State& s) {                                                 \
        auto data = mk_##tname(N);                                                                                     \
        using Var = std::variant<int, llong, double, ldouble, float, ullong>;                                          \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto v : data) {                                                                                      \
                Var a{v};                                                                                              \
                sum += std::get<T>(a);                                                                                 \
            }                                                                                                          \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdVariant_RoundTrip_##tname);
BENCH_RT(int, int)
BENCH_RT(llong, ll)
BENCH_RT(double, dbl)
BENCH_RT(ldouble, ldbl)
#undef BENCH_RT

BENCHMARK_MAIN();