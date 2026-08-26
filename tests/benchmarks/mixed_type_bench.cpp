// Benchmark: Mixed-type operations �?cross-type arithmetic & comparison
// Compares numeric_any vs native. Extended: int, long long, ull, float, double, long double
#include <benchmark/benchmark.h>
#define DISABLE_FORMAT_IN_NUMERIC_ANY
#include "../../src/numeric_any.hpp"
#include <any>
#include <random>
#include <variant>
#include <vector>


using namespace cy::maths;
static constexpr size_t N = 1000000;

using llong   = long long;
using ullong  = unsigned long long;
using ldouble = long double;

static std::vector<int> make_ints(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> d(-100000, 100000);
    std::vector<int> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<llong> make_llongs(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<llong> d(-10000000000LL, 10000000000LL);
    std::vector<llong> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<ullong> make_ullongs(size_t n) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<ullong> d(0, 18000000000000000000ULL);
    std::vector<ullong> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<float> make_floats(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> d(-1000.f, 1000.f);
    std::vector<float> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<double> make_doubles(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> d(-1000., 1000.);
    std::vector<double> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}
static std::vector<ldouble> make_ldoubles(size_t n) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<ldouble> d(-1000.L, 1000.L);
    std::vector<ldouble> v(n);
    for (auto& x : v)
        x = d(rng);
    return v;
}

// ── Construct (with variant for types in variant list) ──
#define BENCH_CONSTRUCT_V(T, tname)                                                                                    \
    static void BM_Native_Construct_##tname(benchmark::State& s) {                                                     \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(T{v});                                                                        \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Construct_##tname);                                                                            \
    static void BM_NumericAny_Construct_##tname(benchmark::State& s) {                                                 \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(numeric_any{v});                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Construct_##tname);                                                                        \
    static void BM_StdAny_Construct_##tname(benchmark::State& s) {                                                     \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(std::any{v});                                                                 \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdAny_Construct_##tname);                                                                            \
    static void BM_StdVariant_Construct_##tname(benchmark::State& s) {                                                 \
        auto data = make_##tname##s(N);                                                                                \
        using Var = std::variant<int, llong, ullong, float, double, ldouble>;                                          \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(Var{v});                                                                      \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdVariant_Construct_##tname);
BENCH_CONSTRUCT_V(int, int)
BENCH_CONSTRUCT_V(llong, llong)
BENCH_CONSTRUCT_V(double, double)
BENCH_CONSTRUCT_V(ldouble, ldouble)
#undef BENCH_CONSTRUCT_V

// ── Construct (no variant �?ullong, float not in variant) ──
#define BENCH_CONSTRUCT_NV(T, tname)                                                                                   \
    static void BM_Native_Construct_##tname(benchmark::State& s) {                                                     \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(T{v});                                                                        \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Construct_##tname);                                                                            \
    static void BM_NumericAny_Construct_##tname(benchmark::State& s) {                                                 \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(numeric_any{v});                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Construct_##tname);                                                                        \
    static void BM_StdAny_Construct_##tname(benchmark::State& s) {                                                     \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s)                                                                                               \
            for (auto v : data)                                                                                        \
                benchmark::DoNotOptimize(std::any{v});                                                                 \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_StdAny_Construct_##tname);
BENCH_CONSTRUCT_NV(ullong, ullong)
BENCH_CONSTRUCT_NV(float, float)
#undef BENCH_CONSTRUCT_NV

// ── Add ──
#define BENCH_ADD_INT(T, tname, V0)                                                                                    \
    static void BM_Native_Add_##tname(benchmark::State& s) {                                                           \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s) {                                                                                             \
            T sum = 0;                                                                                                 \
            for (auto v : data)                                                                                        \
                sum += v;                                                                                              \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Add_##tname);                                                                                  \
    static void BM_NumericAny_Add_##tname(benchmark::State& s) {                                                       \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s) {                                                                                             \
            numeric_any sum{V0};                                                                                       \
            for (auto v : data)                                                                                        \
                sum += v;                                                                                              \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Add_##tname);
BENCH_ADD_INT(int, int, 0)
BENCH_ADD_INT(llong, llong, 0LL)
BENCH_ADD_INT(ullong, ullong, 0ULL)
#undef BENCH_ADD_INT

// ── Mul ──
#define BENCH_MUL_FLOAT(T, tname, V1)                                                                                  \
    static void BM_Native_Mul_##tname(benchmark::State& s) {                                                           \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s) {                                                                                             \
            T acc = 1;                                                                                                 \
            for (auto v : data)                                                                                        \
                acc *= v;                                                                                              \
            benchmark::DoNotOptimize(acc);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Mul_##tname);                                                                                  \
    static void BM_NumericAny_Mul_##tname(benchmark::State& s) {                                                       \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s) {                                                                                             \
            numeric_any acc{V1};                                                                                       \
            for (auto v : data)                                                                                        \
                acc *= v;                                                                                              \
            benchmark::DoNotOptimize(acc);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Mul_##tname);
BENCH_MUL_FLOAT(float, float, 1.0f)
BENCH_MUL_FLOAT(double, double, 1.0)
BENCH_MUL_FLOAT(ldouble, ldouble, 1.0L)
#undef BENCH_MUL_FLOAT

// ── Compare ──
#define BENCH_COMPARE(T, tname)                                                                                        \
    static void BM_Native_Compare_##tname(benchmark::State& s) {                                                       \
        auto data = make_##tname##s(N);                                                                                \
        for (auto _ : s) {                                                                                             \
            int lt = 0, eq = 0, gt = 0;                                                                                \
            for (size_t i = 0; i + 1 < N; i += 2) {                                                                    \
                auto r = data[i] <=> data[i + 1];                                                                      \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r == 0)                                                                                       \
                    ++eq;                                                                                              \
                else                                                                                                   \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + eq + gt);                                                                    \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * (N / 2));                                                                 \
    }                                                                                                                  \
    BENCHMARK(BM_Native_Compare_##tname);                                                                              \
    static void BM_NumericAny_Compare_##tname(benchmark::State& s) {                                                   \
        auto data = make_##tname##s(N);                                                                                \
        std::vector<numeric_any> na;                                                                                   \
        na.reserve(N);                                                                                                 \
        for (auto v : data)                                                                                            \
            na.emplace_back(v);                                                                                        \
        for (auto _ : s) {                                                                                             \
            int lt = 0, eq = 0, gt = 0;                                                                                \
            for (size_t i = 0; i + 1 < N; i += 2) {                                                                    \
                auto r = na[i] <=> na[i + 1];                                                                          \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r == 0)                                                                                       \
                    ++eq;                                                                                              \
                else                                                                                                   \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + eq + gt);                                                                    \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * (N / 2));                                                                 \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Compare_##tname);                                                                          \
    static void BM_StdVariant_Compare_##tname(benchmark::State& s) {                                                   \
        auto data = make_##tname##s(N);                                                                                \
        using Var = std::variant<int, llong, float, double, ldouble>;                                                  \
        std::vector<Var> vars;                                                                                         \
        vars.reserve(N);                                                                                               \
        for (auto v : data)                                                                                            \
            vars.emplace_back(v);                                                                                      \
        for (auto _ : s) {                                                                                             \
            int lt = 0, eq = 0, gt = 0;                                                                                \
            for (size_t i = 0; i + 1 < N; i += 2) {                                                                    \
                auto r =                                                                                               \
                    std::visit([](auto a, auto b) -> std::partial_ordering { return a <=> b; }, vars[i], vars[i + 1]); \
                if (r < 0)                                                                                             \
                    ++lt;                                                                                              \
                else if (r == 0)                                                                                       \
                    ++eq;                                                                                              \
                else                                                                                                   \
                    ++gt;                                                                                              \
            }                                                                                                          \
            benchmark::DoNotOptimize(lt + eq + gt);                                                                    \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * (N / 2));                                                                 \
    }                                                                                                                  \
    BENCHMARK(BM_StdVariant_Compare_##tname);
BENCH_COMPARE(int, int)
BENCH_COMPARE(llong, llong)
BENCH_COMPARE(double, double)
BENCH_COMPARE(ldouble, ldouble)
#undef BENCH_COMPARE

// ── Get ──
#define BENCH_GET(T, tname)                                                                                            \
    static void BM_NumericAny_Get_##tname(benchmark::State& s) {                                                       \
        auto data = make_##tname##s(N);                                                                                \
        std::vector<numeric_any> na;                                                                                   \
        na.reserve(N);                                                                                                 \
        for (auto v : data)                                                                                            \
            na.emplace_back(v);                                                                                        \
        for (auto _ : s) {                                                                                             \
            ldouble sum = 0;                                                                                           \
            for (auto& a : na)                                                                                         \
                sum += as<T>(a);                                                                                       \
            benchmark::DoNotOptimize(sum);                                                                             \
        }                                                                                                              \
        s.SetItemsProcessed(s.iterations() * N);                                                                       \
    }                                                                                                                  \
    BENCHMARK(BM_NumericAny_Get_##tname);                                                                              \
    static void BM_StdAny_Get_##tname(benchmark::State& s) {                                                           \
        auto data = make_##tname##s(N);                                                                                \
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
    BENCHMARK(BM_StdAny_Get_##tname);                                                                                  \
    static void BM_StdVariant_Get_##tname(benchmark::State& s) {                                                       \
        auto data = make_##tname##s(N);                                                                                \
        using Var = std::variant<int, llong, double, ldouble>;                                                         \
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
    BENCHMARK(BM_StdVariant_Get_##tname);
BENCH_GET(int, int)
BENCH_GET(llong, llong)
BENCH_GET(double, double)
BENCH_GET(ldouble, ldouble)
#undef BENCH_GET

// ── Sub / Div ──
static void BM_Native_Sub_Int(benchmark::State& s) {
    auto data = make_ints(N);
    for (auto _ : s) {
        int acc = 0;
        for (auto v : data)
            acc -= v;
        benchmark::DoNotOptimize(acc);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Native_Sub_Int);
static void BM_NumericAny_Sub_Int(benchmark::State& s) {
    auto data = make_ints(N);
    for (auto _ : s) {
        numeric_any acc{0};
        for (auto v : data)
            acc -= v;
        benchmark::DoNotOptimize(acc);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_NumericAny_Sub_Int);
static void BM_Native_Div_Double(benchmark::State& s) {
    auto data = make_doubles(N);
    for (auto _ : s) {
        double acc = 1e6;
        for (auto v : data)
            acc /= (v == 0.0 ? 1.0 : v);
        benchmark::DoNotOptimize(acc);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_Native_Div_Double);
static void BM_NumericAny_Div_Double(benchmark::State& s) {
    auto data = make_doubles(N);
    for (auto _ : s) {
        numeric_any acc{1e6};
        for (auto v : data)
            acc /= (v == 0.0 ? 1.0 : v);
        benchmark::DoNotOptimize(acc);
    }
    s.SetItemsProcessed(s.iterations() * N);
}
BENCHMARK(BM_NumericAny_Div_Double);

BENCHMARK_MAIN();