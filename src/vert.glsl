#version 330 core

precision mediump float;

layout(location = 0) in uvec2 index;
layout(location = 1) in uint char_;

uniform vec2  RESOLUTION;
uniform uvec2 PIXELS;
uniform uvec2 CELLS;

#define SIZE_CELL vec2(PIXELS / CELLS)
#define K         24.0
#define SCALE     (SIZE_CELL * K)

out vec2      VERT_OUT_POSITION;
flat out uint VERT_OUT_CHAR;

void main() {
    VERT_OUT_CHAR = char_;
    vec2 coord = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
    VERT_OUT_POSITION = coord;
    coord = (coord * SCALE) + (index * SCALE);
    coord.y = -coord.y;
    gl_Position = vec4((coord / RESOLUTION) + vec2(-1.0, 1.0), 0.0, 1.0);
}
