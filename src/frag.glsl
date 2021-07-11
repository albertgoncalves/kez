#version 330 core

precision mediump float;

in vec2 VERT_OUT_POSITION;

uniform float TIME;
uniform vec2  RESOLUTION;
uniform uvec2 PIXELS;
uniform uvec2 CELLS;

#define SIZE_CELL vec2(PIXELS / CELLS)

uniform sampler2D TEXTURE;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

void main() {
    vec2 index = uvec2(1, 1) / vec2(PIXELS);
    vec2 coord = vec2(VERT_OUT_POSITION.x, VERT_OUT_POSITION.y);
    vec3 color = vec3(gl_FragCoord.xy / RESOLUTION, 0.5);
    color *= (sin(TIME * 0.1) + 1.0) / 2.0;
    FRAG_OUT_COLOR = texture(TEXTURE, (coord / CELLS) + (index * SIZE_CELL)) *
                     vec4(1.0 - color, 1.0);
}
