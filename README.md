# ⏱️ ubench.h

[![Build status](https://ci.appveyor.com/api/projects/status/8x6h7ji9wpmh3rdd?svg=true)](https://ci.appveyor.com/project/sheredom/ubench-h)
[![Build Status](https://travis-ci.org/sheredom/ubench.h.svg)](https://travis-ci.org/sheredom/ubench.h)

A simple one header solution to benchmarking for C/C++.

## Usage

Just `#include "ubench.h"` in your code!

The current supported compilers are gcc, clang and msvc.

The current supported platforms are Linux, macOS and Windows.

## Command Line Options

ubench.h supports some command line options:

* `--help` to output the help message
* `--filter=<filter>` will filter the benchmarks to run (useful for re-running
  one particular offending benchmark).
* `--list-tests` will list benchmark names, one per line. Output names can be
  passed to `--filter`.
* `--output=<output>`will output a CSV file of the results.
* `--max-deviation=<deviation>` will change the maximum deviation cut-off for a
  failed test.

## Design

UBench is a single header library to enable all the fun of benchmarking in C and
C++. The library has been designed to provide an output similar to Google's
googletest framework:

```
[==========] Running 1 benchmarks.
[ RUN      ] foo.bar
[       OK ] foo.bar (102289906ns +- 1.751264%)
[==========] 1 benchmarks ran.
[  PASSED  ] 1 benchmarks
```

Benchmarks can fail if their standard deviation exceeds a certain threshold. The
default threshold is 2.5% - anything above this is usually a sign that there is
too much variance to trust the benchmark result.

## UBENCH_MAIN

In one C or C++ file, you must call the macro UBENCH_MAIN:

```c
UBENCH_MAIN();
```

This will call into ubench.h, instantiate all the benchmarks and run the
benchmark framework.

Alternatively, if you want to write your own main and call into ubench.h, you
can instead, in one C or C++ file call:

```c
UBENCH_STATE();
```

And then when you are ready to call into the ubench.h framework do:

```c
int main(int argc, const char *const argv[]) {
  // do your own thing
  return ubench_main(argc, argv);
}
```

## Define a Benchmark

To define a benchmark to run, you can do the following;

```c
#include "ubench.h"

UBENCH(foo, bar) {
  usleep(100 * 1000);
}
```

The UBENCH macro takes two parameters - the first being the set that the
benchmark belongs to, the second being the name of the benchmark. This allows
benchmarks to be grouped for convenience.

## Testing Macros

We currently provide the following macros to be used within UBENCHs:

### UBENCH_DO_NOTHING(x)

The helper macro `UBENCH_DO_NOTHING` takes a single pointer argument. It's
purpose is to ensure that the compiler safetly does nothing with the pointer -
meaning that the compiler cannot optimize the data away. This is incredibly
useful for benchmarks because they generally want to run some code for timing
and not have the compiler optimize the code away.

## License

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
