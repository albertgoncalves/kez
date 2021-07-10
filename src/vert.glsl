#version 330 core

precision mediump float;

out vec2 VERT_OUT_POSITION;

void main() {
    float x = float(gl_VertexID & 1);
    float y = float((gl_VertexID >> 1) & 1);
    VERT_OUT_POSITION = vec2(x, y);
    gl_Position = vec4((vec2(x, y) - 0.5) * 2.0, 0.0, 1.0);
}
