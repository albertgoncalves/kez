#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef uint8_t  u8;
typedef uint32_t u32;

typedef int32_t i32;

typedef float  f32;
typedef double f64;

#define null nullptr

typedef time_t    Epoch;
typedef struct tm Time;

#define EXIT_WITH(x)                                                        \
    {                                                                       \
        fprintf(stderr, "%s:%s:%d\n%s\n", __FILE__, __func__, __LINE__, x); \
        exit(EXIT_FAILURE);                                                 \
    }

#define EXIT_IF(condition)    \
    if (condition) {          \
        EXIT_WITH(#condition) \
    }

#endif
