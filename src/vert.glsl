#version 330 core

precision mediump float;

layout(location = 0) in uvec2 index;

uniform vec2  RESOLUTION;
uniform uvec2 PIXELS;
uniform uvec2 CELLS;

#define SIZE_CELL vec2(PIXELS / CELLS)

out vec2 VERT_OUT_POSITION;

#define SCALE 20.0

void main() {
    vec2 coord = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
    VERT_OUT_POSITION = coord;
    vec2 scale = SIZE_CELL * SCALE;
    coord = (coord * scale) + (index * scale);
    coord.y = -coord.y;
    gl_Position = vec4((coord / RESOLUTION) + vec2(-1.0, 1.0), 0.0, 1.0);
}
