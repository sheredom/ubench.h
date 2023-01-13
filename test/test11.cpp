/*
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
*/

#include "ubench.h"

// Only enable the test if we aren't using Visual Studio, or we're using Visual
// Studio 2019.
#if !defined(_MSC_VER) || (_MSC_VER >= 1920)

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#include <chrono>
#include <thread>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

UBENCH(cpp11, tenth_of_a_second) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
#endif

UBENCH(cpp11, do_nothing) {
  static char a[8 * 1024 * 1024];
  static char b[8 * 1024 * 1024];
  UBENCH_DO_NOTHING(a);
  memcpy(b, a, sizeof(a));
  UBENCH_DO_NOTHING(b);
}

UBENCH_EX(cpp11, ex) {
  int b[1024];
  int i;
  int sum;
  memset(b, 0x0, sizeof(b));

  UBENCH_DO_BENCHMARK() {
    sum = 0;
    for (i = 0; i < 1024; ++i)
      sum += i;
  }

  UBENCH_DO_NOTHING(&sum);
}

struct cpp11_my_fixture {
  char *data;
};

UBENCH_F_SETUP(cpp11_my_fixture) {
  const int size = 128 * 1024 * 1024;
  ubench_fixture->data = static_cast<char *>(malloc(size));
  memset(ubench_fixture->data, ' ', size - 1);
  ubench_fixture->data[size - 1] = '\0';
  ubench_fixture->data[size / 2] = 'f';
}

UBENCH_F_TEARDOWN(cpp11_my_fixture) { free(ubench_fixture->data); }

UBENCH_F(cpp11_my_fixture, strchr) {
  UBENCH_DO_NOTHING(strchr(ubench_fixture->data, 'f'));
}

UBENCH_F(cpp11_my_fixture, strrchr) {
  UBENCH_DO_NOTHING(strrchr(ubench_fixture->data, 'f'));
}

UBENCH_EX_F(cpp11_my_fixture, strchr_ex) {
  char data[128 * 4];
  memcpy(data, ubench_fixture->data, sizeof(data));
  data[sizeof(data) - 1] = '\0';

  UBENCH_DO_BENCHMARK() { UBENCH_DO_NOTHING(strchr(data, 'f')); }
}
