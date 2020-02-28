# okon - overpowered key occurence nailer
Utilities for blazing fast offline searching for SHA1 keys in Have I Been Pwned databases.

(Actually okon can handle any file that is formatted like HIBP files, which is in every line:
a SHA1 hash, a colon and some number, e.g. `0000000000000000000000000000000000000000:68181`)

# How it works
Before you can search for you SHA1 hash in the database, an one-time special step has to be done.
You need to process the downloaded database. Then, when it's done, you can search for hashes in the prepared file.
Please check [usage](#Usage) section for details.

# Utilities
okon consists of two things: a library and binary.

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
