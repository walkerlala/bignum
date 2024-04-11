#include "decimal.h"

#include <benchmark/benchmark.h>
#include <cstdint>

using namespace bignum;

static void small_int64_addition(benchmark::State &state) {
        int64_t a = 123;
        int64_t b = 567;
        for (auto _ : state) {
                int64_t c = a + b;
                benchmark::DoNotOptimize(c);
                benchmark::ClobberMemory();
        }
}

static void small_decimal_zero_scale_addition(benchmark::State &state) {
        Decimal a("123");
        Decimal b("567");
        for (auto _ : state) {
                Decimal c = a + b;
                benchmark::DoNotOptimize(c);
                benchmark::ClobberMemory();
        }
}

BENCHMARK(small_int64_addition);
BENCHMARK(small_decimal_zero_scale_addition);
