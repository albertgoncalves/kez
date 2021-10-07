#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef size_t   usize;

typedef int32_t i32;

typedef float  f32;
typedef double f64;

#define null nullptr

typedef time_t    Epoch;
typedef struct tm Time;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define EXIT_WITH(x)                                                        \
    {                                                                       \
        fprintf(stderr, "%s:%s:%d\n%s\n", __FILE__, __func__, __LINE__, x); \
        _exit(EXIT_FAILURE);                                                \
    }

#define EXIT_IF(condition)    \
    if (condition) {          \
        EXIT_WITH(#condition) \
    }

#endif
