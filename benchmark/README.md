# Benchmarks

The goal is to benchmark end-to-end solutions. Solutions that user could use to check whether their hash is present in the original file.

**That's why, please note**:
`grep` operates on the original file.
`okon` takes advantage of file preprocessing - it is needed to 'prepare' the original file before using okon (see [preparing example](https://github.com/stryku/okon#command-line-interface).

# Okon benchmarking
okon implements B-tree to handle hashes. `okon_btree_benchmark` target is provided to check okon's performance.

## BEWARE
`okon_btree_benchmark` and `okon_grep_benchmark` need root privileges to execute `'sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"'`, in order to clear OS cache.
It's executed before every attempt to find a hash by okon.

## Creating `okon_btree_benchmark` target
To create `okon_btree_benchmark` target, set `OKON_WITH_BTREE_BENCHMARK` option to `ON`.

Additional arguments are necessary:
* `OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK` - how many hashes you'd want to benchmark.
* `OKON_BENCHMARK_ORIGINAL_DB` - a path to the original hashes database file.
* `OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB` - how many hashes are in the original file.
* (optional) `OKON_BENCHMARK_SEED` - seed which should be used to choose random hashes from original database file. If it's not provided, `0` is set.

# `grep` solution benchmarking
`okon_grep_benchmark` target is provided to check solution with grepping over the original file.
Of course, it doesn't exist to benchmark the grep. It's more of getting an idea of how much time grep needs in an average case.

## Creating `okon_grep_benchmark` target
To create `okon_grep_benchmark` target, set `OKON_WITH_GREP_BENCHMARK` option to `ON`.

Additional arguments are necessary:
* `OKON_BENCHMARK_NUMBER_OF_HASHES_TO_BENCHMARK` - how many hashes you'd want to benchmark.
* `OKON_BENCHMARK_ORIGINAL_DB` - a path to the original hashes database file.
* `OKON_BENCHMARK_NUMBER_OF_HASHES_IN_ORIGINAL_DB` - how many hashes are in the original file.
* (optional) `OKON_BENCHMARK_SEED` - seed which should be used to choose random hashes from original database file. If it's not provided, `0` is set.
