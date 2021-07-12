#include "init.hpp"
#include "init_assets_codegen.hpp"
#include "scene_assets_codegen.hpp"

#include <unistd.h>

#define CAP_GLYPHS 20

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
    char  chars[CAP_GLYPHS];
    Glyph glyphs[CAP_GLYPHS];
};

struct Arena {
    BufferMemory buffer_memory;
    GlyphMemory  glyph_memory;
};

static Object OBJECT;

#define MICROSECONDS 1000000.0f

#define FRAME_DURATION ((1.0f / 60.0f) * MICROSECONDS)

#define CHECK_GL_ERROR()                               \
    {                                                  \
        switch (glGetError()) {                        \
        case GL_INVALID_ENUM: {                        \
            ERROR("GL_INVALID_ENUM");                  \
        }                                              \
        case GL_INVALID_VALUE: {                       \
            ERROR("GL_INVALID_VALUE");                 \
        }                                              \
        case GL_INVALID_OPERATION: {                   \
            ERROR("GL_INVALID_OPERATION");             \
        }                                              \
        case GL_INVALID_FRAMEBUFFER_OPERATION: {       \
            ERROR("GL_INVALID_FRAMEBUFFER_OPERATION"); \
        }                                              \
        case GL_OUT_OF_MEMORY: {                       \
            ERROR("GL_OUT_OF_MEMORY");                 \
        }                                              \
        case GL_NO_ERROR: {                            \
            break;                                     \
        }                                              \
        }                                              \
    }

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wmissing-noreturn"

static void error_callback(i32 code, const char* error) {
    fprintf(stderr, "%d: %s\n", code, error);
    exit(EXIT_FAILURE);
}

#pragma GCC diagnostic pop

static void set_glyphs(GlyphMemory* memory) {
    {
        Epoch epoch;
        time(&epoch);
        Time* time = localtime(&epoch);
        snprintf(memory->chars,
                 CAP_GLYPHS,
                 "%4d-%02d-%02d %02d:%02d:%02d",
                 1900 + time->tm_year,
                 time->tm_mon,
                 time->tm_mday,
                 time->tm_hour,
                 time->tm_min,
                 time->tm_sec);
    }
    {
        for (u32 i = 0; i < CAP_GLYPHS; ++i) {
            Glyph* glyph = &memory->glyphs[i];
            glyph->char_ = static_cast<u32>(memory->chars[i]);
            glyph->position.x = i + 1;
            glyph->position.y = 1;
        }
    }
}

i32 main() {
    printf("GLFW version  : %s\n"
           "sizeof(Glyph) : %zu\n"
           "sizeof(Arena) : %zu\n"
           "\n",
           glfwGetVersionString(),
           sizeof(Glyph),
           sizeof(Arena));
    Arena* arena = reinterpret_cast<Arena*>(calloc(1, sizeof(Arena)));
    EXIT_IF(!arena);
    glfwSetErrorCallback(error_callback);
    EXIT_IF(!glfwInit());
    GLFWwindow* window = init_get_window("float");
    const u32   program = init_get_program(
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
        {
            glActiveTexture(GL_TEXTURE0);
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
        };
        glUniform2ui(uniform.pixels, PIXEL_WIDTH, PIXEL_HEIGHT);
        // NOTE: Is there any way to avoid hard-coding this information?
        glUniform2ui(uniform.cells, 18, 7);
        CHECK_GL_ERROR();
        while (!glfwWindowShouldClose(window)) {
            const f32 time = static_cast<f32>(glfwGetTime());
            {
                glfwPollEvents();
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(window, true);
                }
            }
            {
                set_glyphs(&arena->glyph_memory);
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
                glUniform1f(uniform.time, time);
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, CAP_GLYPHS);
                CHECK_GL_ERROR();
            }
            glfwSwapBuffers(window);
            {
                const f32 now = static_cast<f32>(glfwGetTime());
                const f32 elapsed = (now - time) * MICROSECONDS;
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
    free(arena);
    return EXIT_SUCCESS;
}
