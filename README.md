
# okon - overpowered key occurrence nailer

![Travis CI](https://travis-ci.org/stryku/okon.svg?branch=master)

![okon](https://github.com/stryku/okon/blob/master/images/okon_logo.png)

Utilities for fast offline searching for SHA-1 keys in [Have I Been Pwned databases](https://haveibeenpwned.com/Passwords).

(Actually, okon can handle any text file has an SHA-1 hash at the beginning of every line)

# Table of Contents
- [okon - overpowered key occurence nailer](#okon---overpowered-key-occurence-nailer)
- [Table of Contents](#table-of-contents)
- [Benchmarks](#benchmarks)
- [How it works](#how-it-works)
- [Utilities](#utilities)
- [Usage](#usage)
  * [Library](#library)
  * [Command line interface](#command-line-interface)
- [How it really works](#how-it-really-works)
- [Building](#building)


# Benchmarks
Benchmarks are based on database version 5, form [HIBP page](https://haveibeenpwned.com/Passwords) (8e1c0f161a756e409ec51a6fceefdc63d34cea01).

Benchmarks are done on my PC (Intel(R) Core(TM) i7-6700 CPU @ 3.40GHz, 16GB RAM), on HDD (not SSD).
`okon` and `C++ line by line` are done using [google/benchmark](https://github.com/google/benchmark) utils. You can find them in [benchmarks file](https://github.com/stryku/okon/blob/master/benchmark/exists_benchmark.cpp).

Worst case. Find the hash that is on the last place in an original database:

|                  |   time [μs] |           % |
|-----------------:|------------:|------------:|
|             okon |          49 |         100 |
| C++ line by line | 135'720'201 | 276'980'002 |
|     grep '^hash' | 195'350'000 | 398'673'469 |
|             grep | 195'480'000 | 398'938'775 |

# How it works
Before you search for a SHA-1 hash in the database, the database needs to be processed. With the processed file, okon is able to quickly search for keys.
Please check [usage](#Usage) section for details.

# Utilities
okon consists of two things: a library and a binary.

* Library: a library with C language interface. It can be easily integrated into an existing codebase.
* Binary: `okon-cli` is a binary that implements command line interface. You can do everything what the `okon` library can.

# Usage
## Library
If you have an existing codebase and you'd want to integrate okon, just build the binary and link to it in your code.
For documentation check out [the header file](https://github.com/stryku/okon/blob/master/include/okon/okon.h).

## Command line interface
To process a file downloaded from HIBP:
```
okon-cli --prepare path/to/downloaded/file.txt --wd path/to/working_directory --output path/to/prepared/file.okon
```

To search for a key in the prepared file:
```
okon-cli --path path/to/prepared/file.okon --hash 0000000000000000000000000000000000000000
```
If the hash is present `okon-cli` will write `1` to stdout and set exit code to 1.
If the hash is NOT present `okon-cli` will write `0` to stdout and set exit code to 0.

# How it really works
We're lucky guys. SHA1 hashes have two very, very nice traits. They are comparable and all of them are of the same size \o/

Thanks to that, having a bunch of hashes we're able to create a B-tree out of them. And that's exactly what happens in the 'preparing step'. You take several hundred million hashes and insert them in a file which is logically represented as a B-tree.
Then, even with a couple-GB file, searching is really fast.

# Building
CMake and C++17 are required.

CMake options:
- `OKON_WITH_CLI=ON/OFF` - Build okon-cli binary.
- `OKON_WITH_TESTS=ON/OFF` - Build tests.
- `OKON_WITH_HEAVY_TEST=ON/OFF` - Add target for heavy test (requires python3). Heavy test takes original database, prepares okon's file, iterates over all hashes in original db and verifies that it's findable in prepared file. If `OKON_WITH_HEAVY_TEST` is set to ON:
  * `OKON_HEAVY_TEST_ORIGINAL_DB=path/to/file` - Path to a file containing original HIBP database, over which the heavy test should be run.
- `OKON_WITH_BENCHMARKS=ON/OFF` - Build benchmarks (requires googlebenchmark). If `OKON_WITH_BENCHMARKS` is set to ON:
  * `OKON_BENCHMARK_FILE=path/to/file` - Path to a file based on which benchmarks have to be run.
  * `OKON_BENCHMARK_BEST_CASE=SHA-1` - Value of the first hash in file specified in `OKON_BENCHMARK_FILE`
  * `OKON_BENCHMARK_WORST_CASE=SHA-1` - Value to the last hash in file specified in `OKON_BENCHMARK_FILE`
  * `OKON_BENCHMARK_BTREE_FILE=path/to/file` - Path to a file prepared by okon, that should be used in benchmarks.
- 

Building routine:
```sh
git clone https://github.com/stryku/okon
mkdir build && cd build
cmake ../okon -DOKON_WITH_CLI=ON -DCMAKE_CXX_COMPILER=<COMPILER SUPPORTING C++17> -DCMAKE_INSTALL_PREFIX=../../install
make install

# Verify that okon-cli has been built
cd ../../install/bin
./okon-cli --help
```
