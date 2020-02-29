#include <benchmark/benchmark.h>

#include <btree.hpp>
#include <fstream_wrapper.hpp>

namespace {
constexpr auto k_sha1_length{ 40u };

bool exists_substring(std::string_view sha1_to_search, std::string_view file_path)
{
  std::ifstream in{ file_path.data() };

  if (!in.is_open()) {
    return false;
  }

  for (std::string line; std::getline(in, line);) {
    const auto sha1_in_line = std::string_view{ line.data(), k_sha1_length };
    if (sha1_to_search == sha1_in_line) {
      return true;
    }
  }

  return false;
}

bool exists_static_buffer_length(std::string_view sha1_to_search, std::string_view file_path)
{
  std::ifstream in{ file_path.data() };

  if (!in.is_open()) {
    return false;
  }

  constexpr auto k_max_line_length{ k_sha1_length + 15u };

  for (std::array<char, k_max_line_length> line{}; in.getline(&line[0], k_max_line_length);) {
    const auto sha1_in_line = std::string_view{ line.data(), k_sha1_length };
    if (sha1_to_search == sha1_in_line) {
      return true;
    }
  }

  return false;
}
}

static void exists_substring_best_case(benchmark::State& state)
{
  while (state.KeepRunning()) {
    const auto result = exists_substring(OKON_BENCHMARK_BEST_CASE, OKON_BENCHMARK_FILE);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_substring_best_case);

static void exists_substring_worst_case(benchmark::State& state)
{
  while (state.KeepRunning()) {
    const auto result = exists_substring(OKON_BENCHMARK_WORST_CASE, OKON_BENCHMARK_FILE);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_substring_worst_case);

static void exists_static_buffer_length_best_case(benchmark::State& state)
{
  while (state.KeepRunning()) {
    const auto result = exists_static_buffer_length(OKON_BENCHMARK_BEST_CASE, OKON_BENCHMARK_FILE);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_static_buffer_length_best_case);

static void exists_static_buffer_length_worst_case(benchmark::State& state)
{
  while (state.KeepRunning()) {
    const auto result = exists_static_buffer_length(OKON_BENCHMARK_WORST_CASE, OKON_BENCHMARK_FILE);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_static_buffer_length_worst_case);

static void exists_btree_best_case(benchmark::State& state)
{
  while (state.KeepRunning()) {
    okon::fstream_wrapper wrapper{ OKON_BENCHMARK_BTREE_FILE };
    okon::btree<okon::fstream_wrapper> tree{ wrapper };
    const auto result = tree.contains(okon::string_sha1_to_binary(OKON_BENCHMARK_BEST_CASE));
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_btree_best_case);

static void exists_btree_worst_case(benchmark::State& state)
{
  while (state.KeepRunning()) {
    okon::fstream_wrapper wrapper{ OKON_BENCHMARK_BTREE_FILE };
    okon::btree<okon::fstream_wrapper> tree{ wrapper };
    const auto result = tree.contains(okon::string_sha1_to_binary(OKON_BENCHMARK_WORST_CASE));
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(exists_btree_worst_case);

BENCHMARK_MAIN();