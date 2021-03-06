#include "init.hpp"
#include "init_assets_codegen.hpp"
#include "scene_assets_codegen.hpp"

#include <sys/mman.h>

#define CAP_CHARS  20
#define CAP_GLYPHS (CAP_CHARS - 1)

static i32 WINDOW_WIDTH = (1 << 10) + (1 << 9) + (1 << 8);
static i32 WINDOW_HEIGHT = (1 << 8) + (1 << 6);

struct Object {
    u32 vertex_array;
    u32 vertex_buffer;
    u32 texture;
};

struct Uniform {
    i32 resolution;
    i32 time;
    i32 pixels;
    i32 cells;
    i32 texture;
};

struct Position {
    u32 x;
    u32 y;
};

struct Glyph {
    Position position;
    u32      char_;
};

struct GlyphMemory {
    char  chars[CAP_CHARS];
    Glyph glyphs[CAP_GLYPHS];
};

struct Arena {
    BufferMemory buffer_memory;
    GlyphMemory  glyph_memory;
};

static Object OBJECT;

static u32 CURSOR;

#define MICROSECONDS 1000000.0f

#define FRAME_DURATION ((1.0f / 60.0f) * MICROSECONDS)

#define CHECK_GL_ERROR()                                   \
    {                                                      \
        switch (glGetError()) {                            \
        case GL_INVALID_ENUM: {                            \
            EXIT_WITH("GL_INVALID_ENUM");                  \
        }                                                  \
        case GL_INVALID_VALUE: {                           \
            EXIT_WITH("GL_INVALID_VALUE");                 \
        }                                                  \
        case GL_INVALID_OPERATION: {                       \
            EXIT_WITH("GL_INVALID_OPERATION");             \
        }                                                  \
        case GL_INVALID_FRAMEBUFFER_OPERATION: {           \
            EXIT_WITH("GL_INVALID_FRAMEBUFFER_OPERATION"); \
        }                                                  \
        case GL_OUT_OF_MEMORY: {                           \
            EXIT_WITH("GL_OUT_OF_MEMORY");                 \
        }                                                  \
        case GL_NO_ERROR: {                                \
            break;                                         \
        }                                                  \
        }                                                  \
    }

[[noreturn]] static void error_callback(i32 code, const char* error) {
    fprintf(stderr, "%d: %s\n", code, error);
    _exit(EXIT_FAILURE);
}

static void key_callback(GLFWwindow* window, i32 key, i32, i32 action, i32) {
    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        if (key == GLFW_KEY_LEFT) {
            if (0 < CURSOR) {
                --CURSOR;
            }
        }
        if (key == GLFW_KEY_RIGHT) {
            if (CURSOR < (CAP_GLYPHS - 1)) {
                ++CURSOR;
            }
        }
    }
}

static void framebuffer_size_callback(GLFWwindow*, i32 width, i32 height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

static void* alloc(usize size) {
    void* memory = mmap(null,
                        size,
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        -1,
                        0);
    EXIT_IF(memory == MAP_FAILED);
    return memory;
}

i32 main() {
    printf("GLFW version  : %s\n"
           "sizeof(Glyph) : %zu\n"
           "sizeof(Arena) : %zu\n"
           "\n",
           glfwGetVersionString(),
           sizeof(Glyph),
           sizeof(Arena));
    Arena* arena = reinterpret_cast<Arena*>(alloc(sizeof(Arena)));
    glfwSetErrorCallback(error_callback);
    EXIT_IF(!glfwInit());
    GLFWwindow* window = init_get_window("float", WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    const u32 program = init_get_program(
        &arena->buffer_memory,
        init_get_shader(&arena->buffer_memory, SHADER_VERT, GL_VERTEX_SHADER),
        init_get_shader(&arena->buffer_memory,
                        SHADER_FRAG,
                        GL_FRAGMENT_SHADER));
    {
        {
            glGenVertexArrays(1, &OBJECT.vertex_array);
            glBindVertexArray(OBJECT.vertex_array);
        }
        {
            glGenBuffers(1, &OBJECT.vertex_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, OBJECT.vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(arena->glyph_memory.glyphs),
                         &arena->glyph_memory.glyphs[0],
                         GL_DYNAMIC_DRAW);
            {
                glEnableVertexAttribArray(0);
                glVertexAttribIPointer(
                    0,
                    2,
                    GL_UNSIGNED_INT,
                    sizeof(Glyph),
                    reinterpret_cast<void*>(offsetof(Glyph, position)));
                glVertexAttribDivisor(0, 1);
            }
            {
                glEnableVertexAttribArray(1);
                glVertexAttribIPointer(
                    1,
                    1,
                    GL_UNSIGNED_INT,
                    sizeof(Glyph),
                    reinterpret_cast<void*>(offsetof(Glyph, char_)));
                glVertexAttribDivisor(1, 1);
            }
            CHECK_GL_ERROR();
        }
        // NOTE: Since we are syncing via the `glUniform1i` call below, we can
        // use this arbitrary index value.
        const i32 texture_index = 1;
        {
            glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(texture_index));
            glGenTextures(1, &OBJECT.texture);
            glBindTexture(GL_TEXTURE_2D, OBJECT.texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_S,
                            GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_T,
                            GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         PIXEL_WIDTH,
                         PIXEL_HEIGHT,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         PIXEL_DATA);
            CHECK_GL_ERROR();
        }
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(program);
        const Uniform uniform = {
            glGetUniformLocation(program, "RESOLUTION"),
            glGetUniformLocation(program, "TIME"),
            glGetUniformLocation(program, "PIXELS"),
            glGetUniformLocation(program, "CELLS"),
            glGetUniformLocation(program, "TEXTURE"),
        };
        CHECK_GL_ERROR();
        glUniform2ui(uniform.pixels, PIXEL_WIDTH, PIXEL_HEIGHT);
        // NOTE: Is there any way to avoid hard-coding this information?
        glUniform2ui(uniform.cells, 18, 7);
        glUniform1i(uniform.texture, texture_index);
        CHECK_GL_ERROR();
        while (!glfwWindowShouldClose(window)) {
            const f32 time_ = static_cast<f32>(glfwGetTime());
            glfwPollEvents();
            {
                {
                    Epoch epoch;
                    time(&epoch);
                    const Time* local_time = localtime(&epoch);
                    snprintf(arena->glyph_memory.chars,
                             CAP_CHARS,
                             "%4d-%02d-%02d %02d:%02d:%02d",
                             local_time->tm_year + 1900,
                             local_time->tm_mon + 1,
                             local_time->tm_mday,
                             local_time->tm_hour,
                             local_time->tm_min,
                             local_time->tm_sec);
                    for (u32 i = 0; i < CAP_GLYPHS; ++i) {
                        arena->glyph_memory.glyphs[i].char_ =
                            static_cast<u32>(arena->glyph_memory.chars[i]);
                        arena->glyph_memory.glyphs[i].position.x = i + 1;
                        arena->glyph_memory.glyphs[i].position.y = 1;
                    }
                    arena->glyph_memory.glyphs[CURSOR].char_ |= 0x100;
                }
                glBufferSubData(GL_ARRAY_BUFFER,
                                0,
                                sizeof(arena->glyph_memory.glyphs),
                                &arena->glyph_memory.glyphs[0]);
            }
            {
                glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                glUniform2f(uniform.resolution,
                            static_cast<f32>(WINDOW_WIDTH),
                            static_cast<f32>(WINDOW_HEIGHT));
                glUniform1f(uniform.time, time_);
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, CAP_GLYPHS);
                CHECK_GL_ERROR();
                glfwSwapBuffers(window);
            }
            {
                const f32 elapsed =
                    (static_cast<f32>(glfwGetTime()) * MICROSECONDS) -
                    (time_ * MICROSECONDS);
                if (elapsed < FRAME_DURATION) {
                    usleep(static_cast<u32>(FRAME_DURATION - elapsed));
                }
            }
        }
    }
    {
        glDeleteProgram(program);
        glDeleteVertexArrays(1, &OBJECT.vertex_array);
        glDeleteBuffers(1, &OBJECT.vertex_buffer);
        glDeleteTextures(1, &OBJECT.texture);
        CHECK_GL_ERROR();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
