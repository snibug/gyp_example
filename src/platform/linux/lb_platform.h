/*
 * Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SRC_PLATFORM_LINUX_LB_PLATFORM_H_
#define SRC_PLATFORM_LINUX_LB_PLATFORM_H_

// Some of the platform code is implementing chromium APIs, some of it is
// emulating POSIX APIs, and this file is for everything else.

#include <byteswap.h>
#include <inttypes.h>  // uint64_t
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
namespace LB {
namespace Platform {
#endif

static inline uint64_t TickCount() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  uint64_t absolute_micro =
    (int64_t)(ts.tv_sec) * 1000000 +
    (int64_t)(ts.tv_nsec) / 1000;

  return absolute_micro;
}

static inline void DEBUG_BREAK() {
#if defined(NDEBUG)
  abort();
#else
  __builtin_trap();
#endif
}

static inline int BeingDebugged() {
#if defined(NDEBUG)
  return 0;
#else
  return 1;
#endif
}

static inline uint32_t atomic_inc_32(uint32_t *addend) {
  return __sync_add_and_fetch(addend, 1);
}

static inline uint32_t atomic_dec_32(uint32_t *addend) {
  return __sync_sub_and_fetch(addend, 1);
}

static inline uint32_t atomic_add_32(uint32_t *addend, uint32_t add) {
  return __sync_add_and_fetch(addend, add);
}

static inline uint32_t atomic_sub_32(uint32_t *addend, uint32_t sub) {
  return __sync_sub_and_fetch(addend, sub);
}

// Increment addend by 1 if the existing value is not 0.
// Returns the original value of addend.
static inline uint32_t atomic_conditional_inc(uint32_t *addend) {
  uint32_t value = *addend;
  while (1) {
    if (value == 0) {
        return 0;
    }

    uint32_t before = __sync_val_compare_and_swap(addend, value, value + 1);

    if (before == value) {
      return value;
    } else {
      value = before;
    }
  }
}

// === compiler intrinsics for quick and readable byte (re)-ordering

// load bytes pointed to by p and return as value
static inline uint16_t load_uint16_big_endian(const uint8_t* p) {
  return bswap_16(*(uint16_t*)p);
}

static inline uint32_t load_uint32_big_endian(const uint8_t* p) {
  return bswap_32(*(uint32_t*)p);
}

static inline uint64_t load_uint64_big_endian(const uint8_t* p) {
  return bswap_64(*(uint64_t*)p);
}

static inline int16_t load_int16_big_endian(const uint8_t* p) {
  return (int16_t)load_uint16_big_endian(p);
}

static inline int32_t load_int32_big_endian(const uint8_t* p) {
  return (int32_t)load_uint32_big_endian(p);
}

static inline int64_t load_int64_big_endian(const uint8_t* p) {
  return (int64_t)load_uint64_big_endian(p);
}

static inline uint16_t load_uint16_little_endian(const uint8_t* p) {
  return *(uint16_t*)p;
}

static inline uint32_t load_uint32_little_endian(const uint8_t* p) {
  return *(uint32_t*)p;
}

static inline uint64_t load_uint64_little_endian(const uint8_t* p) {
  return *(uint64_t*)p;
}

static inline int16_t load_int16_little_endian(const uint8_t* p) {
  return (int16_t)load_uint16_little_endian(p);
}

static inline int32_t load_int32_little_endian(const uint8_t* p) {
  return (int32_t)load_uint32_little_endian(p);
}

static inline int64_t load_int64_little_endian(const uint8_t* p) {
  return (int64_t)load_uint64_little_endian(p);
}

// store the provided value d into the bytes pointed at by p
static inline void store_uint16_big_endian(uint16_t d, uint8_t* p) {
  *((uint16_t*)p) = bswap_16(d);
}

static inline void store_uint32_big_endian(uint32_t d, uint8_t* p) {
  *((uint32_t*)p) = bswap_32(d);
}

static inline void store_uint64_big_endian(uint64_t d, uint8_t* p) {
  *((uint64_t*)p) = bswap_64(d);
}

static inline void store_uint16_little_endian(uint16_t d, uint8_t* p) {
  *(uint16_t*)p = d;
}

static inline void store_uint32_little_endian(uint32_t d, uint8_t* p) {
  *(uint32_t*)p = d;
}

static inline void store_uint64_little_endian(uint64_t d, uint8_t* p) {
  *(uint64_t*)p = d;
}

#ifdef __cplusplus
}  // namespace Platform
}  // namespace LB
#endif

typedef void* lb_physical_mem_t;
typedef uintptr_t lb_virtual_mem_t;

#if defined(__cplusplus)
// Copied from chromium: base/basictypes.h
#if !defined(COMPILE_ASSERT)
template<bool> struct LBCompileAssert { };

#define COMPILE_ASSERT(expr, msg)                    \
  typedef LBCompileAssert<(static_cast<bool>(expr))> \
          msg[static_cast<bool>(expr) ? 1 : -1]
#endif
#endif  // defined (__cplusplus)

#define LB_ALWAYS_INLINE __attribute__((always_inline))

#endif  // SRC_PLATFORM_LINUX_LB_PLATFORM_H_
