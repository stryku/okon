/*
 * A thin layer on top of okon btree::contains() method.
 * It searches for a given hash and measures time of that operation, then writes the time to stdout.
 * It's used by okon_btree_benchmark.py script.
 */

#include <benchmark/benchmark.h>

#include <okon/okon.h>

#include <chrono>

#include <iostream>

int main(int argc, const char* argv[])
{
  typedef std::chrono::high_resolution_clock hr_clock_t;

  if (argc < 3) {
    return 1;
  }

  const auto benchmark_hash = argv[1];
  const auto benchmark_btree_file = argv[2];

  const auto start = hr_clock_t::now();
  const auto result = okon_exists_text(benchmark_hash, benchmark_btree_file);
  const auto end = hr_clock_t::now();

  const auto duration_micros = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << duration_micros.count() << '\n';

  benchmark::DoNotOptimize(result);

  return 0;
}
