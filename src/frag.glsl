#version 330 core

precision mediump float;

in vec2 VERT_OUT_POSITION;

uniform vec2 U_RESOLUTION;

uniform sampler2D TEXTURE;

layout(location = 0) out vec4 FRAG_OUT_COLOR;

void main() {
    FRAG_OUT_COLOR =
        texture(TEXTURE,
                vec2(VERT_OUT_POSITION.x, 1.0 - VERT_OUT_POSITION.y)) *
        vec4(gl_FragCoord.xy / U_RESOLUTION, 0.25, 1.0);
}
