#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t  u8;
typedef uint32_t u32;

typedef int32_t i32;

typedef float f32;

#define null nullptr

#define ERROR(x)                                                            \
    {                                                                       \
        fprintf(stderr, "%s:%s:%d\n%s\n", __FILE__, __func__, __LINE__, x); \
        exit(EXIT_FAILURE);                                                 \
    }

#define EXIT_IF(condition) \
    if (condition) {       \
        ERROR(#condition)  \
    }

#endif
