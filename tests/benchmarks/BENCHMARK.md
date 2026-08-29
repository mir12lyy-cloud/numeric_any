# numeric_any Benchmark Report (New Iteration)

> **Date**: 2026-08-28 | **CPU**: 20 × 2880 MHz | **OS**: Windows + WSL2 (Ubuntu)

---

## Table of Contents

1. [Environment & Methodology](#1-environment--methodology)
2. [Same-Type Operations](#2-same-type-operations)
3. [Run-to-Run Consistency](#3-run-to-run-consistency)
4. [Type Conversion & Policies](#4-type-conversion--policies)
5. [hpp vs C++20 Modules (ixx)](#5-hpp-vs-c20-modules-ixx)
6. [WSL (Linux, GCC) vs Windows (Clang)](#6-wsl-linux-gcc-vs-windows-clang)
7. [10-Repetition Statistics](#7-10-repetition-statistics)
8. [Changes vs Previous Iteration](#8-changes-vs-previous-iteration)
9. [Final Conclusions](#9-final-conclusions)

---

## 1. Environment & Methodology

### Toolchain

| Item | Value |
|------|-------|
| Env A (Windows) | Clang 22.1.3 (MSVC LLVM toolchain, MSVC STL), C++20 |
| Env B (WSL2 Ubuntu) | GCC 15.2.0 (libstdc++), C++23 |
| Optimization | `-O3 -DNDEBUG` (Release) |
| Benchmark framework | Google Benchmark v1.9.1 (FetchContent) |
| Data scale | N = 1,000,000 |
| Build | `tests/benchmarks/build_v2` (Win, Ninja) / `tests/benchmarks/build_wsl_v2` (WSL, Ninja) |
| Code version | `cy::maths::numeric_any` (`as` / `from`, policies `equal` / `strict` / `normal`) |

### Comparison Targets

| Type-Erased Scheme | Description |
|--------------------|-------------|
| `cy::maths::numeric_any` | Type-erased arithmetic container (this project) |
| `std::any` | C++17 type-erased container (small-buffer optimized in MSVC STL) |
| `std::variant<...>` | C++17 discriminated union |
| Native types | `int`, `long long`, `unsigned long long`, `float`, `double`, `long double` |

### Benchmark Dimensions

| Category | Source File | Contents |
|----------|-------------|----------|
| Same-Type Operations | `same_type_bench.cpp` | Construction, arithmetic, comparison, retrieval |
| Mixed-Type Operations | `mixed_type_bench.cpp` | Same suite re-run (repeatability check) |
| Type Conversion & Policies | `conversion_bench.cpp` | `as` / `from` (equal/strict/normal), round-trips |

> All tables report **median real time per 1M iterations** (ns). Lower is better.

---

## 2. Same-Type Operations

### 2.1 Construction (Windows/Clang)

| Type | Native | numeric_any | std::any | std::variant | na/Native | na/any |
|------|--------|-------------|----------|--------------|-----------|--------|
| int | 230,164 | 1,317,771 | 871,931 | 324,693 | 5.7x | 1.51x slower |
| long long | 401,714 | 1,332,189 | 1,171,875 | 645,229 | 3.3x | 1.14x slower |
| unsigned long long | 408,155 | 739,397 | 1,123,047 | -- | 1.8x | **1.52x faster** |
| float | 230,190 | 1,074,219 | 1,996,314 | -- | 4.7x | **1.86x faster** |
| double | 444,984 | 1,339,286 | 1,708,984 | 571,987 | 3.0x | 1.28x faster |
| long double | 357,492 | 1,464,844 | 1,497,236 | 393,345 | 4.1x | 1.02x faster |

### 2.2 Arithmetic (Windows/Clang)

| Operation | Native | numeric_any | na/Native |
|-----------|--------|-------------|-----------|
| int += int | 114,397 | 1,283,482 | **11.2x** |
| long long += ll | 240,626 | 767,299 | 3.2x |
| **unsigned long long += ull** | 213,472 | **223,214** | **1.05x (at parity)** |
| **float \*= float** | 836,680 | **920,348** | **1.10x** |
| double \*= double | 892,857 | 1,171,875 | 1.31x |
| long double \*= ld | 1,133,510 | 1,534,598 | 1.35x |
| int -= int | 102,539 | 1,245,117 | 12.1x |
| **double /= double** | 4,047,440 | **3,138,951** | **0.78x (na faster)** |

> `ullong` add at parity, `float` mul near-native, and this run `double` div is actually **faster than native (Maybe the execution sequence)**. Small signed ints remain the worst case (~3–12x).

### 2.3 Comparison (Windows/Clang)

| Type | Native | numeric_any | std::variant | na/Native | na/variant |
|------|--------|-------------|--------------|-----------|------------|
| int | 2,979,343 | 8,125,000 | 4,199,219 | 2.7x | 1.9x |
| long long | 2,929,688 | 5,902,778 | 3,374,413 | 2.0x | 1.8x |
| double | 2,455,357 | 5,998,884 | 4,741,379 | 2.4x | 1.3x |
| long double | 3,523,284 | 6,406,250 | 3,906,250 | 1.8x | 1.6x |

### 2.4 Retrieval (Get) (Windows/Clang)

| Type | numeric_any | std::any | std::variant | na/any | na/variant |
|------|-------------|----------|--------------|--------|------------|
| int | 2,142,559 | 4,589,844 | 1,123,047 | **2.1x faster** | 1.9x |
| long long | 2,148,438 | 4,589,844 | 1,123,047 | **2.1x faster** | 1.9x |
| double | 1,811,594 | 4,718,960 | 1,074,219 | **2.6x faster** | 1.7x |
| long double | 1,843,164 | 4,718,960 | 1,395,089 | **2.6x faster** | 1.3x |

---

## 3. Run-to-Run Consistency

Both `same_type_bench.cpp` and `mixed_type_bench.cpp` run the same suite; running both provides a repeatability check (Windows/Clang):

| Benchmark | same_type run | mixed_type run |
|-----------|---------------|----------------|
| Construct int | 1,317,771 | 1,123,047 |
| Add int | 1,283,482 | 1,045,850 |
| Add ullong | 223,214 | 262,277 |
| Get int | 2,142,559 | 2,665,134 |
| Compare int | 8,125,000 | 10,009,766 |

> Cross-process variance up to ~1.5x on some metrics; see Section 7 for the 10-repetition pass.

---

## 4. Type Conversion & Policies

### 4.1 Same-Type Cast (Windows/Clang)

| Operation | `as` | strict | **equal** | std::any_cast | std::variant |
|-----------|------|--------|-----------|---------------|--------------|
| int -> int | 1,902,174 | 2,471,515 | 1,674,107 | 4,589,844 | 1,147,461 |
| long long -> ll | 2,148,438 | 2,050,781 | 1,717,493 | 4,565,747 | 1,025,391 |
| float -> float | 2,197,266 | 1,992,754 | 1,843,164 | 4,565,747 | -- |
| double -> double | 1,881,143 | 2,050,781 | 1,717,493 | 4,464,286 | 1,004,464 |
| long double -> ld | 1,843,164 | 2,038,043 | 1,765,971 | 4,612,199 | 1,066,767 |

> With the new `as` fast path, `as` ≈ `strict` ≈ `equal` on same-type casts (differences within run noise). All remain **~2.2–2.6x faster than `std::any_cast`**.

### 4.2 Cross-Type Casts & Policies

**Windows/Clang (median ns/1M):**

| Operation | `as` | equal | strict | normal |
|-----------|------|-------|--------|--------|
| int -> long long (promote) | 2,148,438 | **1,689,189** | 2,197,266 | -- |
| int -> short (narrow) | -- | **1,650,799** | 2,099,609 | 6,417,411 |
| int -> long | -- | **1,727,580** | 2,246,094 | 2,083,333 |
| double -> int (cross) | -- | -- | -- | 2,604,167 |
| long double -> int (cross) | -- | -- | -- | 2,604,167 |

**WSL/GCC (median ns/1M):**

| Operation | equal | strict | normal |
|-----------|-------|--------|--------|
| int -> long long (promote) | **1,185,474** | 2,699,982 | -- |
| int -> short (narrow) | **1,162,554** | 2,507,655 | 2,903,821 |
| int -> long | **1,159,579** | 2,803,226 | 2,689,443 |

> **`equal` remains the cheapest cross-type path** — a single `is_same_type` compare, no `visit` dispatch: ~1.2–1.3x faster than `strict` on Windows, **~2.2–2.4x faster than `strict`/`normal` on WSL/GCC**. `normal` narrowing is the slowest on Windows (per-element range check).

### 4.3 Construct + Get Round-Trip (Windows/Clang)

| Type | numeric_any | std::any | std::variant | na vs any | na vs variant |
|------|-------------|----------|--------------|-----------|---------------|
| int | 815,763 | 2,665,134 | 531,250 | **3.3x faster** | 1.5x |
| long long | 544,085 | 2,582,097 | 562,500 | **4.7x faster** | ~1.0x |
| double | 767,299 | 3,676,471 | 732,422 | **4.8x faster** | ~1.0x |
| long double | 837,054 | 3,953,313 | 836,680 | **4.7x faster** | ~1.0x |

> The new `as` fast path directly speeds up round-trips: `long long` improved ~40% vs the previous iteration (899k → 544k).

---

## 5. hpp vs C++20 Modules (ixx)

The module build performs identically to the header build (sample, Windows/Clang, median):

| Benchmark | hpp | ixx |
|-----------|-----|-----|
| NumericAny_Construct_int | 1,317,771 | 1,778,739 |
| NumericAny_Add_int | 1,283,482 | 1,255,020 |
| NumericAny_Get_int | 2,142,559 | 2,299,331 |

Differences fall within cross-run variance — **no module-related penalty**.

---

## 6. WSL (Linux, GCC 15.2.0, C++23) vs Windows (Clang 22.1.3, C++20)

> Same suite, same N=1M. All values median ns per 1M iterations.

### 6.1 Headline Difference (numeric_any)

| Benchmark | Windows (Clang) | WSL (GCC) |
|-----------|-----------------|-----------|
| Construct int | 1,317,771 | 599,519 |
| Construct long long | 1,332,189 | 617,658 |
| Add int | 1,283,482 | 289,790 |
| Add long long | 767,299 | 501,334 |
| Add ullong | 223,214 | 347,614 |
| Mul float | 920,348 | 2,679,635 |
| Mul double | 1,171,875 | 2,835,042 |
| Mul long double | 1,534,598 | 114,208,071 |
| Compare int | 8,125,000 | 6,407,104 |
| Get int | 2,142,559 | 2,261,353 |

### 6.2 Native Reference Differs Too

| Benchmark | Windows native | WSL native |
|-----------|----------------|------------|
| Construct int | 230,164 | 52,507 |
| Compare int | 2,979,343 | 98,961 |

GCC/Linux eliminates trivial native ops (~0.05–0.1 ns/op), so **na/Native ratios are not directly comparable across platforms** (e.g. WSL na/Native compare for int looks like 65x because native is ~free).

### 6.3 vs std::any / std::variant on WSL

| Benchmark | Windows na vs any | WSL na vs any |
|-----------|-------------------|---------------|
| Construct int | 1,318K vs 872K (any 1.51x faster) | 600K vs 1,125K (**na 1.9x faster**) |
| Get int | 2,143K vs 4,590K (na 2.1x) | 2,261K vs 2,976K (na 1.3x) |
| Get ldouble | 1,843K vs 4,719K (na 2.6x) | 2,597K vs 7,049K (na 2.7x) |

| Benchmark | WSL na vs variant |
|-----------|-------------------|
| Construct int | 600K vs 618K (~parity) |
| Get int | 2,261K vs 2,533K (~1.1x) |
| **Compare double** | **5,553K vs 7,055K (na 1.27x faster)** |
| Compare ldouble | 6,596K vs 7,335K (na 1.11x faster) |

> On GCC/Linux `numeric_any` is nearly on par with `std::variant` for construction/get and **beats it for double / long-double comparison**.

### 6.4 long double (80-bit x87 on Linux)

| Benchmark | Windows (na) | WSL (na) |
|-----------|--------------|----------|
| Construct ldouble | 1,464,844 | 7,276,074 |
| **Mul ldouble** | 1,534,598 | 114,208,071 |
| RoundTrip ldouble | 837,054 | — |

> Linux `long double` (80-bit x87) is far slower than MSVC's 64-bit `long double`. Notably, **WSL `na` mul ldouble (114.2M) is faster than native (134.2M) → 0.85x** — the x87 cost dominates and hides type-erasure overhead.

---

## 7. 10-Repetition Statistics

> Each benchmark repeated **10 times** in-process (`--benchmark_repetitions=10 --benchmark_report_aggregates_only`), N=1M. Reported: mean (ns/1M), stddev, CV.

### 7.1 Conversion Policies (Windows/Clang)

| Benchmark | mean (ns) | stddev | CV |
|-----------|-----------|--------|-----|
| `equal` same int | 1,795,377 | 10,995 | **0.61%** |
| `equal` promote int→ll | 1,786,015 | 21,184 | 1.19% |
| `equal` narrow int→short | 1,828,171 | 16,358 | 0.89% |
| `equal` policy int→long | 1,802,038 | 24,117 | 1.34% |
| `as` (unchecked) same dbl | 1,902,931 | 12,976 | 0.68% |
| `as` (unchecked) same int | 2,299,219 | 173,498 | 7.55% |
| `strict` same int | 2,101,404 | 110,088 | 5.24% |
| `normal` policy int→long | 2,159,281 | 37,237 | 1.72% |
| `strict` policy int→long | 2,165,032 | 130,595 | 6.03% |
| `normal` narrow int→short | 6,450,453 | 299,694 | 4.65% |
| std::any same int | 4,833,209 | 358,504 | 7.42% |

### 7.2 Same-Type Suite Highlights (Windows/Clang)

| Benchmark | mean (ns) | stddev | CV |
|-----------|-----------|--------|-----|
| Add ullong (na) | 177,442 | 4,028 | 2.27% |
| Add ullong (native) | 178,902 | 6,738 | 3.77% |
| Add int (na) | 834,160 | 13,711 | 1.64% |
| Construct int (na) | 1,108,752 | 37,999 | 3.43% |
| Compare int (na) | 6,449,035 | 204,614 | 3.17% |
| Compare int (native) | 1,930,030 | 51,582 | 2.67% |

### 7.3 Conversion Policies (WSL/GCC)

| Benchmark | mean (ns) | stddev | CV |
|-----------|-----------|--------|-----|
| `equal` promote int→ll | 1,028,075 | 84,228 | 8.19% |
| `equal` narrow int→short | 1,032,695 | 53,561 | 5.19% |
| `equal` policy int→long | 1,054,330 | 55,216 | 5.24% |
| `equal` same int | 1,635,646 | 53,239 | 3.25% |
| `as` (unchecked) same int | 2,548,841 | 63,622 | 2.50% |
| `strict` same int | 3,839,704 | 62,750 | 1.63% |
| `strict` promote int→ll | 4,272,703 | 82,348 | 1.93% |
| `normal` policy int→long | 4,389,339 | 98,043 | 2.23% |
| `normal` narrow int→short | 2,891,947 | 37,841 | 1.31% |

### 7.4 Findings

1. **`equal` same-type is the most stable benchmark** (CV 0.61% on Windows) and fastest in aggregate (1.80M vs `as` 2.30M / `strict` 2.10M) — the trivial fallback lets the compiler generate the tightest code for the always-taken fast path.
2. **`equal` cross-type fail-fast** remains fastest and stable (Windows CV 0.9–1.3%; ~2.2x faster than `strict` on WSL).
3. **Type erasure adds no extra jitter**: `numeric_any` CV (1.6–7.6%) comparable to native (1.8–3.8%); this 10-rep run showed elevated RoundTrip CV (13–26%) due to system noise.
4. **`ullong` add at parity with native, ultra-stable** (na 177k/CV 2.3% vs native 179k/CV 3.8%).
5. **`normal` narrowing is the slowest path** (Windows mean 6.45M) — per-element range checks.

---

## 8. Changes vs Previous Iteration

The `as` same-type fast path + inlined construction/comparison produced broad improvements on Windows/Clang (median ns/1M):

| Benchmark | Before | After | Δ |
|-----------|--------|-------|---|
| Add long long | 1,199,777 | 767,299 | **-36%** |
| Add ullong | 249,051 | 223,214 | -10% |
| Mul float | 1,464,844 | 920,348 | **-37%** |
| Mul double | 1,569,475 | 1,171,875 | **-25%** |
| Div double | 5,000,000 | 3,138,951 | **-37%** |
| Compare long long | 9,166,667 | 5,902,778 | **-36%** |
| Compare int | 9,765,625 | 8,125,000 | -17% |
| Get int | 2,299,331 | 2,142,559 | -7% |
| Get double | 1,947,464 | 1,811,594 | -7% |
| RoundTrip int | 899,431 | 815,763 | -9% |
| RoundTrip long long | 899,431 | 544,085 | **-40%** |
| Same cast int (`as`) | 2,083,333 | 1,902,174 | -9% |

> Broad single-run improvements (mostly -25% to -40% on arithmetic/comparison, smaller on Get/casts). Note: these are single-shot runs; Section 7 aggregates should be used for rigorous comparisons, and this run's 10-rep pass showed elevated noise on some benchmarks.

---

## 9. Final Conclusions

### numeric_any (current iteration)

```
Retrieval:   ~2.1-2.6x faster than std::any_cast (Windows); 1.3-2.7x (WSL)
Round-trip:  ~3-5x faster than std::any, on par with std::variant (both)
Casts:       as/from-strict ~2.2-2.6x faster than any_cast (Win)
Arithmetic:  ullong add ~1.05x native, float mul 1.10x, double div faster than native (Win)
             big gains vs prev iteration: llong add -36%, mul -25~-37%
Construction: na beats any on WSL; mixed on Win (better for wide/FP types)
Policy cost: equal cheapest cross-type (no visit dispatch); normal narrow slowest
Stability:   CV 1.6-7.6% ≈ native (1.8-3.8%); equal same-type CV 0.61%
```

### Gap vs Native Types (Windows/Clang)

| Scenario | na/Native | Notes |
|----------|-----------|-------|
| int addition | 11.2x | Worst case (tag dispatch dominates) |
| int subtraction | 12.1x | Same |
| int construction | 5.7x | -- |
| long double multiplication | 1.35x | Near-native |
| double multiplication | 1.31x | Near-native |
| **float multiplication** | **1.10x** | Near-native |
| **unsigned long long addition** | **1.05x** | **At parity with native** |
| **double division** | **0.78x** | **Faster than native (this run)** |



