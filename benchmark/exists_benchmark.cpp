#include <benchmark/benchmark.h>

static void StandardString(benchmark::State& state)
{
  if (state.thread_index == 0) {
    // Prepare
  }
  while (state.KeepRunning()) {
    std::string myString;
  }
  if (state.thread_index == 0) {
    // Teardown
  }
}

BENCHMARK(StandardString);
BENCHMARK_MAIN();