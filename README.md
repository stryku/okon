# okon - overpowered key occurence nailer
Utilities for blazing fast offline searching for SHA1 keys in [Have I Been Pwned databases](https://haveibeenpwned.com/Passwords).

(Actually, okon can handle any file that is formatted like HIBP files, which is in every line:
a SHA1 hash, a colon and some number, e.g. `0000000000000000000000000000000000000000:68181`)

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


# Benchmarks
Benchmarks are based on database version 5, form [HIBP page](https://haveibeenpwned.com/Passwords) (8e1c0f161a756e409ec51a6fceefdc63d34cea01).

Benchmarks are done on my PC (Intel(R) Core(TM) i7-6700 CPU @ 3.40GHz, 16GB RAM), on HDD (NOT SSD).
`okon` and `C++ line by line` are done using [google/benchmark](https://github.com/google/benchmark) utils. You can find them in [benchmarks file](https://github.com/stryku/okon/blob/master/benchmark/exists_benchmark.cpp).

Worst case. Find hash that is on the last place in original database:

|                  |   time [μs] |           % |
|-----------------:|------------:|------------:|
|             okon |          51 |         100 |
|             grep |  34'200'000 |  67'095'660 |
|     grep '^hash' |  38'380'000 |  75'296'241 |
| C++ line by line | 138'535'890 | 271'788'218 |

# How it works
Before you search for a SHA1 hash in the database, the database needs to be processed. With the processed file, okon is able to quickly search for keys.
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

Thanks to that, having a bunch of hashes we're able to create a B-tree out of them. And that's exactly what happens in the 'preparing step'. You take several houndred million hashes and insert them in a file which is logically represented as a B-tree.
Then, even with a couple-GB file searching is really fast.

# Building
CMake and C++17 are required.

CMake options:
* OKON_WITH_CLI - Build okon-cli binary.
* OKON_WITH_BENCHMARKS - Build benchmarks (requires googlebenchmark).
* OKON_WITH_TESTS - Build tests.
* OKON_WITH_HEAVY_TEST - Add target for heavy test. Heavy test takes original database, prepares okon's file, iterates over all hashes in original db and verifies that it's findable in prepared file. 

