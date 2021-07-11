#version 330 core

precision mediump float;

in vec2 VERT_OUT_POSITION;

uniform float TIME;
uniform vec2  RESOLUTION;

uniform sampler2D TEXTURE;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

// NOTE: Is there any way to avoid hard-coding this information?
#define COUNT_PIXELS uvec2(128, 64)
#define COUNT_CELLS  uvec2(18, 7)
#define SIZE_CELL    vec2(COUNT_PIXELS / COUNT_CELLS) / vec2(COUNT_PIXELS)

void main() {
    uvec2 index = uvec2(1, 1);
    vec2  coord = vec2(VERT_OUT_POSITION.x, 1.0 - VERT_OUT_POSITION.y);
    vec3  color = vec3(gl_FragCoord.xy / RESOLUTION, 0.5);
    color *= (sin(TIME * 0.1) + 1.0) / 2.0;
    FRAG_OUT_COLOR =
        texture(TEXTURE, (coord / COUNT_CELLS) + (index * SIZE_CELL)) *
        vec4(1.0 - color, 1.0);
}
