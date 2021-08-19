#ifndef __INIT_H__
#define __INIT_H__

#include "prelude.hpp"

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

struct BufferMemory {
    char buffer[1 << 10];
};

static GLFWwindow* init_get_window(const char* name, i32 width, i32 height) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, name, null, null);
    if (!window) {
        glfwTerminate();
        EXIT_WITH("!window");
    }
    glfwMakeContextCurrent(window);
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
        EXIT_WITH(memory->buffer);
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
        EXIT_WITH(memory->buffer);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

#endif
