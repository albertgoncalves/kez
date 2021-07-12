#version 330 core

precision mediump float;

in vec2      VERT_OUT_POSITION;
flat in uint VERT_OUT_CHAR;

uniform float TIME;
uniform vec2  RESOLUTION;
uniform uvec2 PIXELS;
uniform uvec2 CELLS;

#define SIZE_CELL vec2(PIXELS / CELLS)

uniform sampler2D TEXTURE;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

void main() {
    uint char = (VERT_OUT_CHAR & 0xFFu) - 32u;
    if (126u < char) {
        char = 0u;
    }
    vec2 index = uvec2(char % CELLS.x, char / CELLS.x) / vec2(PIXELS);
    vec2 coord = vec2(VERT_OUT_POSITION.x, VERT_OUT_POSITION.y);
    vec3 color = vec3(gl_FragCoord.xy / RESOLUTION, 0.5);
    color *= (sin(TIME * 0.75) + 1.0) / 2.0;
    vec4 color_alpha = vec4(1.0 - color, 1.0);
    FRAG_OUT_COLOR = (texture(TEXTURE, (coord / CELLS) + (index * SIZE_CELL)) *
                      color_alpha);
    if (bool((VERT_OUT_CHAR & 0x100u) >> 8)) {
        FRAG_OUT_COLOR = color_alpha - FRAG_OUT_COLOR;
    }
}
