//
// Created by Paul Motte on 2019-05-04.
//

#ifndef PTB_TYPES_H
#define PTB_TYPES_H

#include <cstdint>
#include <cstdio>

// @todo: Find a better place for it
#ifndef __has_builtin         // Optional of course.
# define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

#if __has_builtin(__builtin_trap)
# define ASSERT(Expression) if(!(Expression)) {__builtin_trap();}
#else
# define ASSERT(Expression) if(!(Expression)) {*(int *)0 = 0;}
#endif

#define INTERNAL static
#define LOCAL_PERSIST static
#define GLOBAL static

typedef uint8_t		byte;

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef int32		bool32;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef intptr_t	intptr;
typedef uintptr_t	uintptr;

typedef size_t		memory_size;

typedef float		real32;
typedef double		real64;

typedef int8		s8;
typedef int8		s08;
typedef int16		s16;
typedef int32		s32;
typedef int64		s64;
typedef bool32		b32;

typedef uint8		u8;
typedef uint8		u08;
typedef uint16		u16;
typedef uint32		u32;
typedef uint64		u64;

typedef real32		r32;
typedef real64		r64;
typedef real32		f32;
typedef real64		f64;

typedef uintptr_t	umm;
typedef intptr_t	smm;

typedef b32			b32x;
typedef u32			u32x;

// Copied from stdint.h
#define INT8_MAX         127
#define INT16_MAX        32767
#define INT32_MAX        2147483647
#define INT64_MAX        9223372036854775807LL

#define INT8_MIN          -128
#define INT16_MIN         -32768

#define INT32_MIN        (-INT32_MAX-1)
#define INT64_MIN        (-INT64_MAX-1)

#define UINT8_MAX         255
#define UINT16_MAX        65535
#define UINT32_MAX        4294967295U
#define UINT64_MAX        18446744073709551615ULL

#define KILOBYTES(value) ((value) * 1024LL)
#define MEGABYTES(value) (KILOBYTES(value) * 1024LL)
#define GIGABYTES(value) (MEGABYTES(value) * 1024LL)

#endif //LOX_PTB_TYPES_H
