#include "init.hpp"
#include "init_assets_codegen.hpp"
#include "scene_assets_codegen.hpp"

struct Object {
    u32 vertex_array;
    u32 texture;
};

struct Uniform {
    i32 resolution;
};

static Object OBJECT;

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wmissing-noreturn"

static void error_callback(i32 code, const char* error) {
    fprintf(stderr, "%d: %s\n", code, error);
    exit(EXIT_FAILURE);
}

#pragma GCC diagnostic pop

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

i32 main() {
    Memory* memory = reinterpret_cast<Memory*>(calloc(1, sizeof(Memory)));
    EXIT_IF(!memory);
    printf("GLFW version : %s\n\n", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);
    EXIT_IF(!glfwInit());
    GLFWwindow* window = init_get_window("float");
    const u32   program = init_get_program(
        memory,
        init_get_shader(memory, SHADER_VERT, GL_VERTEX_SHADER),
        init_get_shader(memory, SHADER_FRAG, GL_FRAGMENT_SHADER));
    {
        {
            glGenVertexArrays(1, &OBJECT.vertex_array);
            glBindVertexArray(OBJECT.vertex_array);
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
                         WIDTH,
                         HEIGHT,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         PIXELS);
            CHECK_GL_ERROR();
        }
        glClearColor(0.325f, 0.475f, 0.75f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(program);
        const Uniform uniform = {
            glGetUniformLocation(program, "U_RESOLUTION"),
        };
        CHECK_GL_ERROR();
        while (!glfwWindowShouldClose(window)) {
            glfwWaitEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
            {
                glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                glUniform2f(uniform.resolution,
                            static_cast<f32>(WINDOW_WIDTH),
                            static_cast<f32>(WINDOW_HEIGHT));
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                CHECK_GL_ERROR();
            }
            glfwSwapBuffers(window);
            printf("!\n");
        }
    }
    {
        glDeleteProgram(program);
        glDeleteVertexArrays(1, &OBJECT.vertex_array);
        glDeleteTextures(1, &OBJECT.texture);
        CHECK_GL_ERROR();
    }
    glfwTerminate();
    free(memory);
    printf("Done!\n");
    return EXIT_SUCCESS;
}
