# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

cmake_minimum_required(VERSION 3.15)
project(ubench)

include(CTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Base warning flags and whether -std= flags are supported.
# MSVC and clang-cl use /std: and don't accept -std=, so we skip
# the standard flag for those compilers.
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set(UBENCH_C_WARNINGS "-Wall -Wextra -Werror")
  set(UBENCH_CXX_WARNINGS "-Wall -Wextra -Werror")
  set(UBENCH_USE_STD_FLAG TRUE)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(UBENCH_C_WARNINGS "/Wall /WX /wd4514 /wd5045")
    set(UBENCH_CXX_WARNINGS "/Wall /WX /wd4514 /wd5045")
    set(UBENCH_USE_STD_FLAG FALSE)
  else()
    set(UBENCH_C_WARNINGS "-Wall -Wextra -Weverything -Werror -Wno-poison-system-directories")
    set(UBENCH_CXX_WARNINGS "-Wall -Wextra -Weverything -Werror -Wno-poison-system-directories")
    set(UBENCH_USE_STD_FLAG TRUE)
  endif()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set(UBENCH_C_WARNINGS "/Wall /WX /wd4514 /wd5045")
  set(UBENCH_CXX_WARNINGS "/Wall /WX /wd4514 /wd5045")
  set(UBENCH_USE_STD_FLAG FALSE)
else()
  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
endif()

# Helper: set compile flags for a C source file (warnings + standard).
function(ubench_set_c_flags FILE STANDARD)
  if(UBENCH_USE_STD_FLAG)
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${UBENCH_C_WARNINGS} ${STANDARD}")
  else()
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${UBENCH_C_WARNINGS}")
  endif()
endfunction()

# Helper: set compile flags for a C++ source file (warnings + standard + extras).
function(ubench_set_cxx_flags FILE STANDARD)
  if(UBENCH_USE_STD_FLAG)
    set(EXTRA_FLAGS ${ARGN})
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${UBENCH_CXX_WARNINGS} ${STANDARD} ${EXTRA_FLAGS}")
  else()
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${UBENCH_CXX_WARNINGS}")
  endif()
endfunction()

# C files — each compiled under a specific C standard.
ubench_set_c_flags(main.c  "-std=gnu89")
ubench_set_c_flags(test.c  "-std=gnu89")
ubench_set_c_flags(test99.c "-std=gnu99")

list(FIND CMAKE_C_COMPILE_FEATURES c_std_11 IDX)
if(${IDX} GREATER -1)
  ubench_set_c_flags(test11.c "-std=gnu11")
endif()

list(FIND CMAKE_C_COMPILE_FEATURES c_std_17 IDX)
if(${IDX} GREATER -1)
  ubench_set_c_flags(test17.c "-std=gnu17")
endif()

list(FIND CMAKE_C_COMPILE_FEATURES c_std_23 IDX)
if(${IDX} GREATER -1)
  include(CheckCCompilerFlag)
  check_c_compiler_flag("-std=gnu23" UBENCH_C_HAS_GNU23)
  if(UBENCH_C_HAS_GNU23)
    ubench_set_c_flags(test23.c "-std=gnu23")
  else()
    ubench_set_c_flags(test23.c "-std=gnu2x")
  endif()
endif()

# C++ files — each compiled under a specific C++ standard.
ubench_set_cxx_flags(test.cpp   "" "-Wno-c++98-compat")
ubench_set_cxx_flags(test98.cpp "-std=gnu++98")
ubench_set_cxx_flags(test11.cpp "-std=c++11" "-Wno-c++98-compat")
ubench_set_cxx_flags(test14.cpp "-std=c++14" "-Wno-c++98-compat")
ubench_set_cxx_flags(test17.cpp "-std=c++17" "-Wno-c++98-compat")
ubench_set_cxx_flags(test20.cpp "-std=c++20" "-Wno-c++98-compat")

list(FIND CMAKE_CXX_COMPILE_FEATURES cxx_std_23 IDX)
if(${IDX} GREATER -1)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-std=c++23" UBENCH_CXX_HAS_CPP23)
  if(UBENCH_CXX_HAS_CPP23)
    ubench_set_cxx_flags(test23.cpp "-std=c++23" "-Wno-c++98-compat")
  else()
    ubench_set_cxx_flags(test23.cpp "-std=c++2x" "-Wno-c++98-compat")
  endif()
endif()

set(UBENCH_TEST_SOURCES
  ../ubench.h
  main.c
  test.c
  test99.c
  test11.c
  test17.c
  test23.c
  test.cpp
  test98.cpp
  test11.cpp
  test14.cpp
  test17.cpp
  test20.cpp
  test23.cpp
)

# Helper: add a test executable and register it with CTest.
function(ubench_add_test NAME)
  add_executable(${NAME} ${UBENCH_TEST_SOURCES})
  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}> --confidence=100)
endfunction()

ubench_add_test(ubench_test)

# Whole-program-optimization variant.
ubench_add_test(ubench_test_wpo)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(ubench_test_wpo PRIVATE "$<$<CONFIG:RELEASE>:/GL>")
  target_link_options(ubench_test_wpo PRIVATE "$<$<CONFIG:RELEASE>:/LTCG>")
endif()

# Some sanitizer runtimes can't initialise on every host even when
# the toolchain links them fine — e.g. clang's TSan and MSan on
# aarch64 only ship shadow-memory layouts for 39-, 42- and 48-bit
# user VMAs, but Raspberry Pi OS' kernel is configured with
# CONFIG_ARM64_VA_BITS=47. The binary builds, but aborts before
# main with "FATAL: ThreadSanitizer: unsupported VMA range". Probe
# each candidate sanitizer with try_run and skip the ones whose
# runtime can't start. Result is cached so reconfigure is cheap;
# delete CMakeCache.txt to re-probe (e.g. after changing kernels).
function(ubench_check_sanitizer_runs SAN OUT_VAR)
  set(CACHE_VAR "UBENCH_SAN_${SAN}_RUNS")
  if(DEFINED ${CACHE_VAR})
    set(${OUT_VAR} ${${CACHE_VAR}} PARENT_SCOPE)
    return()
  endif()

  set(PROBE_SRC "${CMAKE_BINARY_DIR}/ubench_san_probe_${SAN}.c")
  file(WRITE "${PROBE_SRC}" "#include <stdint.h>\nint main(void) { return 0; }\n")

  try_run(RUN_RESULT COMPILE_RESULT
    "${CMAKE_BINARY_DIR}/ubench_san_probe_${SAN}"
    "${PROBE_SRC}"
    COMPILE_DEFINITIONS "-fsanitize=${SAN}" "-fno-omit-frame-pointer"
    LINK_OPTIONS "-fsanitize=${SAN}"
  )

  if(COMPILE_RESULT AND "${RUN_RESULT}" STREQUAL "0")
    set(RESULT TRUE)
    message(STATUS "Sanitizer ${SAN}: usable")
  else()
    set(RESULT FALSE)
    message(STATUS "Sanitizer ${SAN}: skipping "
                   "(compile=${COMPILE_RESULT}, run=${RUN_RESULT})")
  endif()

  set(${CACHE_VAR} ${RESULT} CACHE INTERNAL
      "Whether the ${SAN} sanitizer runtime starts on this host")
  set(${OUT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang" AND NOT "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
  set(UBENCH_SANITIZER_CANDIDATES
    address
    undefined
    thread
  )

  if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
    set(UBENCH_SANITIZER_CANDIDATES ${UBENCH_SANITIZER_CANDIDATES} memory)
  endif()

  set(UBENCH_SANITIZERS)
  foreach(UBENCH_SANITIZER ${UBENCH_SANITIZER_CANDIDATES})
    if(CMAKE_CROSSCOMPILING)
      list(APPEND UBENCH_SANITIZERS ${UBENCH_SANITIZER})
    else()
      ubench_check_sanitizer_runs(${UBENCH_SANITIZER} UBENCH_SAN_RUNS)
      if(UBENCH_SAN_RUNS)
        list(APPEND UBENCH_SANITIZERS ${UBENCH_SANITIZER})
      endif()
    endif()
  endforeach()

  foreach(UBENCH_SANITIZER ${UBENCH_SANITIZERS})
    ubench_add_test(ubench_${UBENCH_SANITIZER}_test)
    target_compile_options(ubench_${UBENCH_SANITIZER}_test PRIVATE -fsanitize=${UBENCH_SANITIZER} -fno-omit-frame-pointer)
    target_link_options(ubench_${UBENCH_SANITIZER}_test PRIVATE -fsanitize=${UBENCH_SANITIZER})
  endforeach()
endif()
