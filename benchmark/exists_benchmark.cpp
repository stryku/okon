#include <benchmark/benchmark.h>

#include <btree.hpp>
#include <fstream_wrapper.hpp>
#include <pwned_sonic.hpp>

static void exists_substring(benchmark::State& state)
{
  while (state.KeepRunning()) {
    const auto result =
      okon::exists_substring("0E115FEEAB9474B9D680E5528024201AF6E7722F", OKON_BENCHMARK_FILE);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_substring);

static void exists_static_buffer_length(benchmark::State& state)
{
  while (state.KeepRunning()) {
    const auto result = okon::exists_static_buffer_length(
      "0E115FEEAB9474B9D680E5528024201AF6E7722F", OKON_BENCHMARK_FILE);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_static_buffer_length);

// static void exists_splitted_sorted(benchmark::State& state)
//{
//  while (state.KeepRunning()) {
//    const auto result = okon::exists_splitted_sorted(PWNED_SONIC_BENCHMARK_PREPARED_DIR,
//                                                      "0E115FEEAB9474B9D680E5528024201AF6E7722F");
//    benchmark::DoNotOptimize(result);
//  }
//}
// BENCHMARK(exists_splitted_sorted);

static void exists_btree(benchmark::State& state)
{
  while (state.KeepRunning()) {
    okon::fstream_wrapper wrapper{
      "/media/stryku/tb_disk/haveibeenpwned/prepared_full_btree/btree"
    };
    okon::btree<okon::fstream_wrapper> tree{ wrapper };
    const auto result = tree.contains(okon::to_sha1("0E115FEEAB9474B9D680E5528024201AF6E7722F"));
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_btree);

BENCHMARK_MAIN();