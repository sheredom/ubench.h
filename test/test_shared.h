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

/* This header is included by the per-standard test files after they define:
     UBENCH_SUITE     - the suite name token (e.g. c, cpp, cpp11)
     UBENCH_FIXTURE   - the fixture type token (e.g. c_my_fixture)

   The wrapper macros expand UBENCH_SUITE/UBENCH_FIXTURE before passing
   them to the ubench.h macros (which use ## and would otherwise block
   expansion of a macro argument). */

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunsafe-buffer-usage-in-libc-call")
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
#endif
#if __has_warning("-Wunsafe-buffer-usage")
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
#endif

#if !defined(_MSC_VER)
#include <unistd.h>
#endif

#if !defined(UBENCH_SUITE)
#error "UBENCH_SUITE must be defined before including test_shared.h"
#endif

#if !defined(UBENCH_FIXTURE)
#error "UBENCH_FIXTURE must be defined before including test_shared.h"
#endif

/* Wrapper macros: expand the argument first, then pass to ubench.h. */
#define UBENCH_WRAP(set, name) UBENCH(set, name)
#define UBENCH_EX_WRAP(set, name) UBENCH_EX(set, name)
#define UBENCH_F_WRAP(fx, name) UBENCH_F(fx, name)
#define UBENCH_EX_F_WRAP(fx, name) UBENCH_EX_F(fx, name)
#define UBENCH_F_SETUP_WRAP(fx) UBENCH_F_SETUP(fx)
#define UBENCH_F_TEARDOWN_WRAP(fx) UBENCH_F_TEARDOWN(fx)

#if !defined(_MSC_VER)
UBENCH_WRAP(UBENCH_SUITE, tenth_of_a_second) { usleep(1000); }
#endif

UBENCH_WRAP(UBENCH_SUITE, do_nothing) {
  static char a[8 * 1024 * 1024];
  static char b[8 * 1024 * 1024];
  UBENCH_DO_NOTHING(a);
  memcpy(b, a, sizeof(a));
  UBENCH_DO_NOTHING(b);
}

UBENCH_EX_WRAP(UBENCH_SUITE, ex) {
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

#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunsafe-buffer-usage")
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
#endif

UBENCH_F_SETUP_WRAP(UBENCH_FIXTURE) {
  const int size = 128 * 1024 * 1024;
  ubench_fixture->data = UBENCH_PTR_CAST(char *, malloc(size));
  memset(ubench_fixture->data, ' ', size - 1);
  ubench_fixture->data[size - 1] = '\0';
  ubench_fixture->data[size / 2] = 'f';
}

UBENCH_F_TEARDOWN_WRAP(UBENCH_FIXTURE) { free(ubench_fixture->data); }

UBENCH_F_WRAP(UBENCH_FIXTURE, strchr) {
  UBENCH_DO_NOTHING(strchr(ubench_fixture->data, 'f'));
}

UBENCH_F_WRAP(UBENCH_FIXTURE, strrchr) {
  UBENCH_DO_NOTHING(strrchr(ubench_fixture->data, 'f'));
}

UBENCH_EX_F_WRAP(UBENCH_FIXTURE, strchr_ex) {
  char data[128 * 4];
  memcpy(data, ubench_fixture->data, sizeof(data));
  data[sizeof(data) - 1] = '\0';

  UBENCH_DO_BENCHMARK() { UBENCH_DO_NOTHING(strchr(data, 'f')); }
}

UBENCH_EX_WRAP(UBENCH_SUITE, skip) {
  UBENCH_SKIP();
  /* Should never be reached */
  UBENCH_DO_BENCHMARK() {}
}

UBENCH_EX_F_WRAP(UBENCH_FIXTURE, skip) {
  (void)ubench_fixture;
  UBENCH_SKIP();
  /* Should never be reached */
  UBENCH_DO_BENCHMARK() {}
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
