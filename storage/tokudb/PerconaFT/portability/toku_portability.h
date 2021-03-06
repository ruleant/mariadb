/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
// vim: ft=cpp:expandtab:ts=8:sw=4:softtabstop=4:
#ident "$Id$"
/*======
This file is part of PerconaFT.


Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved.

    PerconaFT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2,
    as published by the Free Software Foundation.

    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.

----------------------------------------

    PerconaFT is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License, version 3,
    as published by the Free Software Foundation.

    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.
======= */

#ident "Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved."

#pragma once

#include "toku_config.h"

// Percona portability layer

#if defined(__clang__)
#  define constexpr_static_assert(a, b)
#else
#  define constexpr_static_assert(a, b) static_assert(a, b)
#endif

#if defined(_MSC_VER)
#  error "Windows is not supported."
#endif

#define DEV_NULL_FILE "/dev/null"

// include here, before they get deprecated
#include <toku_atomic.h>

#if defined(__GNUC__)
// GCC linux

#define DO_GCC_PRAGMA(x) _Pragma (#x)

#include <toku_stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>

#if __FreeBSD__
#include <stdarg.h>
#endif

#if defined(HAVE_ALLOCA_H)
# include <alloca.h>
#endif

#if defined(__cplusplus)
# include <type_traits>
#endif

#if defined(__cplusplus)
# define cast_to_typeof(v) (decltype(v))
#else
# define cast_to_typeof(v) (__typeof__(v))
#endif

#else // __GNUC__ was not defined, so...
#  error "Must use a GNUC-compatible compiler."
#endif

// Define some constants for Yama in case the build-machine's software is too old.
#if !defined(HAVE_PR_SET_PTRACER)
/*
 * Set specific pid that is allowed to ptrace the current task.
 * A value of 0 mean "no process".
 */
// Well defined ("Yama" in ascii)
#define PR_SET_PTRACER 0x59616d61
#endif
#if !defined(HAVE_PR_SET_PTRACER_ANY)
#define PR_SET_PTRACER_ANY ((unsigned long)-1)
#endif

#if defined(__cplusplus)
// decltype() here gives a reference-to-pointer instead of just a pointer,
// just use __typeof__
# define CAST_FROM_VOIDP(name, value) name = static_cast<__typeof__(name)>(value)
#else
# define CAST_FROM_VOIDP(name, value) name = cast_to_typeof(name) (value)
#endif

#ifndef TOKU_OFF_T_DEFINED
#define TOKU_OFF_T_DEFINED
typedef int64_t toku_off_t;
#endif

#include "toku_os.h"
#include "toku_htod.h"
#include "toku_assert.h"
#include "toku_crash.h"

#define UU(x) x __attribute__((__unused__))

#if defined(__cplusplus)
extern "C" {
#endif

// Deprecated functions.
#if !defined(TOKU_ALLOW_DEPRECATED)
int      creat(const char *pathname, mode_t mode)   __attribute__((__deprecated__));
int      fstat(int fd, struct stat *buf)            __attribute__((__deprecated__));
int      stat(const char *path, struct stat *buf)   __attribute__((__deprecated__));
int      getpid(void)                               __attribute__((__deprecated__));
#    if defined(__FreeBSD__) || defined(__APPLE__)
int syscall(int __sysno, ...)             __attribute__((__deprecated__));
#    else
long int syscall(long int __sysno, ...)             __attribute__((__deprecated__));
#    endif
 long int sysconf(int)                   __attribute__((__deprecated__));
int      mkdir(const char *pathname, mode_t mode)   __attribute__((__deprecated__));
int      dup2(int fd, int fd2)                      __attribute__((__deprecated__));
int      _dup2(int fd, int fd2)                     __attribute__((__deprecated__));
// strdup is a macro in some libraries.
#undef strdup
#    if defined(__FreeBSD__)
char*    strdup(const char *)         __malloc_like __attribute__((__deprecated__));
#    elif defined(__APPLE__)
char*    strdup(const char *)         __attribute__((__deprecated__));
#    else
char*    strdup(const char *)         __THROW __attribute_malloc__ __nonnull ((1)) __attribute__((__deprecated__));
#    endif
#undef __strdup
char*    __strdup(const char *)         __attribute__((__deprecated__));
#    ifndef DONT_DEPRECATE_WRITES
ssize_t  write(int, const void *, size_t)           __attribute__((__deprecated__));
ssize_t  pwrite(int, const void *, size_t, off_t)   __attribute__((__deprecated__));
#endif
#    ifndef DONT_DEPRECATE_MALLOC
#     if defined(__FreeBSD__)
extern void *malloc(size_t)                    __malloc_like __attribute__((__deprecated__));
extern void free(void*)                        __attribute__((__deprecated__));
extern void *realloc(void*, size_t)            __malloc_like __attribute__((__deprecated__));
#     elif defined(__APPLE__)
extern void *malloc(size_t)                    __attribute__((__deprecated__));
extern void free(void*)                        __attribute__((__deprecated__));
extern void *realloc(void*, size_t)            __attribute__((__deprecated__));
#     else
extern void *malloc(size_t)                    __THROW __attribute__((__deprecated__));
extern void free(void*)                        __THROW __attribute__((__deprecated__));
extern void *realloc(void*, size_t)            __THROW __attribute__((__deprecated__));
#     endif
#    endif
#    ifndef DONT_DEPRECATE_ERRNO
//extern int errno __attribute__((__deprecated__));
#    endif
#if !defined(__APPLE__)
// Darwin headers use these types, we should not poison them
#undef TRUE
#undef FALSE
# pragma GCC poison u_int8_t
# pragma GCC poison u_int16_t
# pragma GCC poison u_int32_t
# pragma GCC poison u_int64_t
# pragma GCC poison BOOL
# pragma GCC poison FALSE
# pragma GCC poison TRUE
#endif
#pragma GCC poison __sync_fetch_and_add
#pragma GCC poison __sync_fetch_and_sub
#pragma GCC poison __sync_fetch_and_or
#pragma GCC poison __sync_fetch_and_and
#pragma GCC poison __sync_fetch_and_xor
#pragma GCC poison __sync_fetch_and_nand
#pragma GCC poison __sync_add_and_fetch
#pragma GCC poison __sync_sub_and_fetch
#pragma GCC poison __sync_or_and_fetch
#pragma GCC poison __sync_and_and_fetch
#pragma GCC poison __sync_xor_and_fetch
#pragma GCC poison __sync_nand_and_fetch
#pragma GCC poison __sync_bool_compare_and_swap
#pragma GCC poison __sync_val_compare_and_swap
#pragma GCC poison __sync_synchronize
#pragma GCC poison __sync_lock_test_and_set
#pragma GCC poison __sync_release
#endif

#if defined(__cplusplus)
};
#endif

void *os_malloc(size_t) __attribute__((__visibility__("default")));
// Effect: See man malloc(2)

void *os_malloc_aligned(size_t /*alignment*/, size_t /*size*/) __attribute__((__visibility__("default")));
// Effect: Perform a malloc(size) with the additional property that the returned pointer is a multiple of ALIGNMENT.
// Requires: alignment is a power of two.


void *os_realloc(void*,size_t) __attribute__((__visibility__("default")));
// Effect: See man realloc(2)

void *os_realloc_aligned(size_t/*alignment*/, void*,size_t) __attribute__((__visibility__("default")));
// Effect: Perform a realloc(p, size) with the additional property that the returned pointer is a multiple of ALIGNMENT.
// Requires: alignment is a power of two.

void os_free(void*) __attribute__((__visibility__("default")));
// Effect: See man free(2)

size_t os_malloc_usable_size(const void *p) __attribute__((__visibility__("default")));
// Effect: Return an estimate of the usable size inside a pointer.  If this function is not defined the memory.cc will
//  look for the jemalloc, libc, or darwin versions of the function for computing memory footprint.

// full_pwrite and full_write performs a pwrite, and checks errors.  It doesn't return unless all the data was written. */
void toku_os_full_pwrite (int fd, const void *buf, size_t len, toku_off_t off) __attribute__((__visibility__("default")));
void toku_os_full_write (int fd, const void *buf, size_t len) __attribute__((__visibility__("default")));

// os_write returns 0 on success, otherwise an errno.
ssize_t toku_os_pwrite (int fd, const void *buf, size_t len, toku_off_t off) __attribute__((__visibility__("default")));
int toku_os_write (int fd, const void *buf, size_t len) __attribute__((__visibility__("default")));

// wrappers around file system calls
FILE * toku_os_fdopen(int fildes, const char *mode);    
FILE * toku_os_fopen(const char *filename, const char *mode);
int toku_os_open(const char *path, int oflag, int mode);
int toku_os_open_direct(const char *path, int oflag, int mode);
int toku_os_close(int fd);
int toku_os_fclose(FILE * stream);
ssize_t toku_os_read(int fd, void *buf, size_t count);
ssize_t toku_os_pread(int fd, void *buf, size_t count, off_t offset);
void toku_os_recursive_delete(const char *path);

// wrapper around fsync
void toku_file_fsync_without_accounting(int fd);
void toku_file_fsync(int fd);
int toku_fsync_directory(const char *fname);

// get the number of fsync calls and the fsync times (total)
void toku_get_fsync_times(uint64_t *fsync_count, uint64_t *fsync_time, uint64_t *long_fsync_threshold, uint64_t *long_fsync_count, uint64_t *long_fsync_time);

void toku_set_func_fsync (int (*fsync_function)(int));
void toku_set_func_pwrite (ssize_t (*)(int, const void *, size_t, toku_off_t));
void toku_set_func_full_pwrite (ssize_t (*)(int, const void *, size_t, toku_off_t));
void toku_set_func_write (ssize_t (*)(int, const void *, size_t));
void toku_set_func_full_write (ssize_t (*)(int, const void *, size_t));
void toku_set_func_fdopen (FILE * (*)(int, const char *));
void toku_set_func_fopen (FILE * (*)(const char *, const char *));
void toku_set_func_open (int (*)(const char *, int, int));
void toku_set_func_fclose(int (*)(FILE*));
void toku_set_func_read(ssize_t (*)(int, void *, size_t));
void toku_set_func_pread (ssize_t (*)(int, void *, size_t, off_t));

int toku_portability_init(void);
void toku_portability_destroy(void);

// Effect: Return X, where X the smallest multiple of ALIGNMENT such that X>=V.
// Requires: ALIGNMENT is a power of two
static inline uint64_t roundup_to_multiple(uint64_t alignment, uint64_t v) {
    return (v + alignment - 1) & ~(alignment - 1);
}

// Branch prediction macros.
// If supported by the compiler, will hint in inctruction caching for likely
// branching. Should only be used where there is a very good idea of the correct
// branch heuristics as determined by profiling. Mostly copied from InnoDB.
// Use:
//   "if (FT_LIKELY(x))" where the chances of "x" evaluating true are higher
//   "if (FT_UNLIKELY(x))" where the chances of "x" evaluating false are higher
#if defined(__GNUC__) && (__GNUC__ > 2) && ! defined(__INTEL_COMPILER)

// Tell the compiler that 'expr' probably evaluates to 'constant'.
#define FT_EXPECT(expr,constant) __builtin_expect(expr, constant)

#else

#warning "No FT branch prediction operations in use!"
#define FT_EXPECT(expr,constant) (expr)

#endif // defined(__GNUC__) && (__GNUC__ > 2) && ! defined(__INTEL_COMPILER)

// Tell the compiler that cond is likely to hold
#define FT_LIKELY(cond) FT_EXPECT(cond, 1)

// Tell the compiler that cond is unlikely to hold
#define FT_UNLIKELY(cond) FT_EXPECT(cond, 0)
