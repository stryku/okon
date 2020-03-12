
# okon - overpowered key occurrence nailer

![Travis CI](https://travis-ci.org/stryku/okon.svg?branch=master)

![okon](https://github.com/stryku/okon/blob/master/images/okon_logo.png)

Utilities for fast offline searching for SHA-1 keys in [Have I Been Pwned databases](https://haveibeenpwned.com/Passwords).

(Actually, okon can handle any text file has an SHA-1 hash at the beginning of every line)

# Table of Contents
- [okon - overpowered key occurrence nailer](#okon---overpowered-key-occurrence-nailer)
- [Table of Contents](#table-of-contents)
- [Benchmarks](#benchmarks)
  * [`okon` results](#-okon--results)
  * [`grep` results](#-grep--results)
- [How it works](#how-it-works)
- [Utilities](#utilities)
- [Usage](#usage)
  * [Library](#library)
  * [Command line interface](#command-line-interface)
- [How it really works](#how-it-really-works)
- [Building](#building)


# Benchmarks
Searching in database version 5, form [HIBP page](https://haveibeenpwned.com/Passwords) (8e1c0f161a756e409ec51a6fceefdc63d34cea01).

Benchmarked searching for 100 random hashes from the original file and calculated average time.
The same hashes are used in `grep` and `okon` benchmarks.

Benchmarks are done on my PC:
* Intel(R) Core(TM) i7-6700 CPU @ 3.40GHz, 16GB RAM.
* SDD: `Model=Crucial_CT275MX300SSD1, FwRev=M0CR031`
* HDD: `Model=WDC WD10EZEX-21WN4A0, FwRev=01.01A01`

All the benchmarks code and more information you can find in [benchmark/ folder](https://github.com/stryku/okon/blob/master/benchmark).

## `okon` results

|     | time [ms] |
|----:|:---------:|
| SSD |   3.405   |
| HDD |  13.779   |


## `grep` results
Of course, I don't want to benchmark the `grep` tool. I don't want to compare it to `okon`, either. These two solutions work differently, so direct comparison is pointless.
It's more of getting an idea of how much time grep needs in an average case.

|     | time [s] |
|----:|:--------:|
| SSD |  26.014  |
| HDD |  70.159  |

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
- `OKON_WITH_BENCHMARKS=ON/OFF` - Build benchmarks (requires googlebenchmark). If `OKON_WITH_BENCHMARKS` is set to ON. Before building benchmarks, please see [benchmarks readme](https://github.com/stryku/okon/blob/master/benchmark/README.md)


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
