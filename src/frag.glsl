#version 330 core

precision mediump float;

in vec2 VERT_OUT_POSITION;

uniform float U_TIME;
uniform vec2  U_RESOLUTION;

uniform sampler2D TEXTURE;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

#define PI 3.1415926538

void main() {
    vec2 coord = vec2(VERT_OUT_POSITION.x, 1.0 - VERT_OUT_POSITION.y);
    vec3 color = vec3(gl_FragCoord.xy / U_RESOLUTION, 0.5);
    color *= (sin(U_TIME * 0.1) + 1.0) / 2.0;
    FRAG_OUT_COLOR = texture(TEXTURE, coord) * vec4(1.0 - color, 1.0);
}
