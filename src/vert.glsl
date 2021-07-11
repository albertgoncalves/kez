#version 330 core

precision mediump float;

out vec2 VERT_OUT_POSITION;

void main() {
    vec2 position =
        vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
    VERT_OUT_POSITION = position;
    gl_Position = vec4((position - 0.5) * 2.0, 0.0, 1.0);
}
