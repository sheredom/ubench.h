# ⏱️ ubench.h

[![Actions Status](https://github.com/sheredom/ubench.h/workflows/CMake/badge.svg)](https://github.com/sheredom/ubench.h/actions)
[![Build status](https://ci.appveyor.com/api/projects/status/8x6h7ji9wpmh3rdd?svg=true)](https://ci.appveyor.com/project/sheredom/ubench-h)
[![Sponsor](https://img.shields.io/badge/💜-sponsor-blueviolet)](https://github.com/sponsors/sheredom)

A simple one header solution to benchmarking for C/C++.

## Usage

Just `#include "ubench.h"` in your code!

The current supported platforms are Linux, macOS and Windows.

The current supported compilers are gcc, clang, MSVC's cl.exe, and clang-cl.exe.

## Command Line Options

ubench.h supports some command line options:

* `--help` to output the help message
* `--filter=<filter>` will filter the benchmarks to run (useful for re-running
  one particular offending benchmark).
* `--list-benchmarks` will list benchmark names, one per line. Output names can be
  passed to `--filter`.
* `--output=<output>`will output a CSV file of the results.
* `--confidence=<confidence>` will change the confidence cut-off for a failed
  test.

## Design

ubench.h is a single header library to enable all the fun of benchmarking in C
and C++. The library has been designed to provide an output similar to Google's
googletest framework:

```
[==========] Running 1 benchmarks.
[ RUN      ] foo.bar
[       OK ] foo.bar (mean 536.235us, confidence interval +- 1.457878%)
[==========] 1 benchmarks ran.
[  PASSED  ] 1 benchmarks
```

Benchmarks can fail if their confidence exceeds a certain threshold. The
default threshold is `2.5%` - anything above this is usually a sign that there
is too much variance to trust the benchmark result. The confidence interval in
use is the [99% confidence interval](http://sphweb.bumc.bu.edu/otlt/MPH-Modules/BS/BS704_Confidence_Intervals/BS704_Confidence_Intervals_print.html).
This means that 99% of the samples will occur within the percentage range of the
mean result. So a mean of `100us` with a confidence interval of `2%` means that
`99%` of the samples will occur in the range `[98us..102us]`. This is a good
measure of a consistent result - it eliminates the `1%` of samples that could
easily skew the standard deviation, while giving us a good confidence on the
data.

You can change the default threshold of `2.5%` by specifying the
`--confidence=<confidence>` option, but it is recommended that you keep a value
that is sufficiently low to enable reproducible results.

The number of iterations ran for each test is proportionate to the time taken to
run each benchmark. The framework aims to run a single set of iterations of the
benchmark in `100ms` - so if you benchmark takes `1ms` to run, it'll start with
100 samples. The number of samples will increase on a failed run (a run of the
benchmark that does not meet the confidence interval cutoff) up to a maximum of
500 samples. At least 10 samples are always done even for longer running tests
to ensure some meaningful confidence interval can be computed.

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

## Define a Benchmark with setup

In some cases it might be beneficial to setup your benchmark state locally in
your benchmark.
This can be done like so:

```c
#include "ubench.h"

UBENCH_EX(foo, short_string) {
  const char* short_string = alloc_test_string(16);

  UBENCH_DO_BENCHMARK() {
    to_test(short_string);
  }

  free(short_string);
}

UBENCH_EX(foo, long_string) {
  const char* long_string = alloc_test_string(16 * 1024);

  UBENCH_DO_BENCHMARK() {
    to_test(long_string);
  }

  free(long_string);
}
```
The benchmark will keep running and measuring within the {} after UBENCH_DO_BENCHMARK()
so all code before and after that scope is only executed once.
This might be the right thing to do in cases where you only want the setup in one benchmark.
If you want some setup for en entire set a 'fixture' might be a better way to go, maybe
you need to do some system-setup for each benchmark in a set.


## Define a Fixtured Benchmark

Fixtures are a way to have some state that is initialized once and then used
throughout a benchmark. They let you take the cost of setting up the benchmark
out of the cost of testing whatever function you want to use it with:

```c
// Declare a struct that holds the state for your benchmark.
struct foo {
  char *data;
};

// Use UBENCH_F_SETUP to initialize your struct (called ubench_fixture in here).
UBENCH_F_SETUP(foo) {
  const int size = 128 * 1024 * 1024;
  ubench_fixture->data = (char *)malloc(size);
  memset(ubench_fixture->data, ' ', size - 2);
  ubench_fixture->data[size - 1] = '\0';
  ubench_fixture->data[size / 2] = 'f';
}

// Use UBENCH_F_TEARDOWM to destroy your struct when complete.
UBENCH_F_TEARDOWN(foo) { free(ubench_fixture->data); }

// Define a benchmark that uses your fixture.
UBENCH_F(foo, strchr) {
  UBENCH_DO_NOTHING(strchr(ubench_fixture->data, 'f'));
}

// You can define as many benchmarks that use the same fixture.
UBENCH_F(foo, strrchr) {
  UBENCH_DO_NOTHING(strrchr(ubench_fixture->data, 'f'));
}
```

Note that fixtures are not guaranteed to be constructed only once - but they are
guaranteed to not impede on the collection of accurate metrics for the running
of your benchmark.

It is also possible to use a fixture in combination with a benchmark with a setup
as follows:

```c
UBENCH_EX_F(my_sys, short_string) {
  const char* short_string = alloc_test_string(16);

  UBENCH_DO_BENCHMARK() {
    my_sys_work(ubench_fixture->sys, short_string));
  }

  free(short_string);
}

UBENCH_EX_F(my_sys, long_string) {
  const char* long_string = alloc_test_string(16 * 1024);

  UBENCH_DO_BENCHMARK() {
    my_sys_work(ubench_fixture->sys, long_string));
  }

  free(long_string);
}
```

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
