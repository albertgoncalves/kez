#ifndef __INIT_H__
#define __INIT_H__

#include "prelude.hpp"

#define GL_GLEXT_PROTOTYPES

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wdocumentation"
#pragma GCC diagnostic ignored "-Wdocumentation-unknown-command"

#include <GLFW/glfw3.h>

#pragma GCC diagnostic pop

struct BufferMemory {
    char buffer[2 << 9];
};

#define INIT_WINDOW_WIDTH  ((1 << 10) + (1 << 9) + (1 << 8))
#define INIT_WINDOW_HEIGHT ((1 << 8) + (1 << 6))

static i32 WINDOW_WIDTH = INIT_WINDOW_WIDTH;
static i32 WINDOW_HEIGHT = INIT_WINDOW_HEIGHT;

static void init_framebuffer_size_callback(GLFWwindow* _,
                                           i32         width,
                                           i32         height) {
    (void)_;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

static GLFWwindow* init_get_window(const char* name) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(INIT_WINDOW_WIDTH,
                                          INIT_WINDOW_HEIGHT,
                                          name,
                                          null,
                                          null);
    if (!window) {
        glfwTerminate();
        ERROR("!window");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, init_framebuffer_size_callback);
    glfwSwapInterval(1);
    return window;
}

static u32 init_get_shader(BufferMemory* memory,
                           const char*   source,
                           u32           type) {
    const u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, null);
    glCompileShader(shader);
    i32 status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(shader,
                           sizeof(memory->buffer),
                           null,
                           memory->buffer);
        ERROR(memory->buffer);
    }
    return shader;
}

static u32 init_get_program(BufferMemory* memory,
                            u32           vertex_shader,
                            u32           fragment_shader) {
    const u32 program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    i32 status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        glGetProgramInfoLog(program,
                            sizeof(memory->buffer),
                            null,
                            memory->buffer);
        ERROR(memory->buffer);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

#endif
